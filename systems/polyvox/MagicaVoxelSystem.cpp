#include "MagicaVoxelSystem.hpp"

#include <filesystem>
#include <unordered_map>
#include <fstream>

#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

#include "kengine.hpp"

#include "data/InstanceComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/PolyVoxComponent.hpp"
#include "data/DefaultShadowComponent.hpp"

#include "functions/OnEntityCreated.hpp"

#include "helpers/assertHelper.hpp"
#include "helpers/instanceHelper.hpp"

#include "string.hpp"
#include "concat.hpp"
#include "Export.hpp"
#include "file_extension.hpp"
#include "MagicaVoxel.hpp"

namespace kengine {
	static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> && volume) noexcept {
		const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encodedMesh);
		return mesh;
	}

	template<typename T>
	static void readFromStream(T & header, std::istream & s) noexcept {
		s.read((char *)&header, sizeof(header));
		kengine_assert(s.gcount() == sizeof(header));
	}

	EntityCreator * MagicaVoxelSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::OnEntityCreated{ onEntityCreated };
			}

			static void onEntityCreated(Entity & e) noexcept {
				if (e.has<ModelComponent>())
					loadModel(e);
				else if (e.has<GraphicsComponent>())
					initObject(e);
			}

			static void initObject(Entity & e) noexcept {
				auto & graphics = e.get<GraphicsComponent>();
				const char * path = graphics.appearance;

				const auto instance = e.tryGet<InstanceComponent>();
				if (instance && instance->model != INVALID_ID) {
					const auto & model = instanceHelper::getModel<ModelComponent>(*instance);
					path = model.file;
				}

				if (putils::file_extension(path) != "vox")
					return;

				e += PolyVoxObjectComponent{};
				e += DefaultShadowComponent{};
			}

			using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>{ {} }));
			struct MagicaVoxelModelComponent {
				MeshType mesh;
			};
			struct MeshInfo {
				MeshType mesh;
				MagicaVoxel::ChunkContent::Size size;
			};

			static void loadModel(Entity & e) noexcept {
				const auto & f = e.get<ModelComponent>().file.c_str();
				if (putils::file_extension(f) != "vox")
					return;

				const putils::string<256> binaryFile("%s.bin", f);

				if (std::filesystem::exists(binaryFile.c_str())) {
					loadBinaryModel(e, binaryFile.c_str());
					return;
				}

#ifndef KENGINE_NDEBUG
				std::cout << putils::termcolor::green << "[MagicaVoxel] Loading " << putils::termcolor::cyan << f << putils::termcolor::green << "..." << putils::termcolor::reset;
#endif
				auto meshInfo = loadVoxModel(f);
				auto & mesh = e.attach<MagicaVoxelModelComponent>().mesh;
				mesh = std::move(meshInfo.mesh);

				auto modelData = generateModelData(e, mesh);
				serialize(binaryFile, modelData, meshInfo.size);
				e += std::move(modelData);

				applyOffset(e, meshInfo.size);

#ifndef KENGINE_NDEBUG
				std::cout << putils::termcolor::green << " Done.\n" << putils::termcolor::reset;
#endif
			}

			static void loadBinaryModel(Entity & e, const char * binaryFile) noexcept {
				MagicaVoxel::ChunkContent::Size size;

				ModelDataComponent modelData;
				modelData.meshes.push_back({});
				unserialize(binaryFile, modelData.meshes.back(), size);
				modelData.free = release(e.id);
				modelData.init<MeshType::VertexType>();
				e += std::move(modelData);

				if (e.has<TransformComponent>())
					return;

				auto & box = e.attach<TransformComponent>().boundingBox;
				box.position.x -= size.x / 2.f * box.size.x;
				box.position.z -= size.y / 2.f * box.size.z;
			}

			static void unserialize(const char * f, ModelDataComponent::Mesh & meshData, MagicaVoxel::ChunkContent::Size & size) noexcept {
				std::ifstream file(f, std::ofstream::binary);
				if (!file) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Failed to load '", f, "'"));
					return;
				}

				const auto parse = [&](auto & val) noexcept {
					file.read((char *)&val, sizeof(val));
				};

				{
					parse(meshData.vertices.nbElements);
					parse(meshData.vertices.elementSize);
					const auto vertexBufferSize = meshData.vertices.nbElements * meshData.vertices.elementSize;
					meshData.vertices.data = new char[vertexBufferSize];
					file.read((char *)meshData.vertices.data, vertexBufferSize);
				}

				{
					parse(meshData.indices.nbElements);
					parse(meshData.indices.elementSize);
					const auto indexBufferSize = meshData.indices.nbElements * meshData.indices.elementSize;
					meshData.indices.data = new char[indexBufferSize];
					file.read((char *)meshData.indices.data, indexBufferSize);
				}

				parse(meshData.indexType);

				parse(size);
			}

			static ModelDataComponent::FreeFunc release(EntityID id) noexcept {
				return [id] {
					auto e = entities[id];
					const auto model = e.tryGet<MagicaVoxelModelComponent>();
					if (model) {
						model->mesh.clear();
						e.detach<MagicaVoxelModelComponent>();
					}
					else { // Was unserialized and we (violently) `new`-ed the data buffers
						const auto & modelData = e.get<ModelDataComponent>();
						delete[] modelData.meshes[0].vertices.data;
						delete[] modelData.meshes[0].indices.data;
					}
				};
			}

			static MeshInfo loadVoxModel(const char * f) noexcept {
				std::ifstream stream(f, std::ios::binary);
				if (!stream) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Failed to load '", f, "'"));
					return MeshInfo{};
				}

				checkHeader(stream);

				MagicaVoxel::ChunkHeader main;
				readFromStream(main, stream);
				if (!idMatches(main.id, "MAIN")) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Expected 'MAIN' chunk header in '", f, "'"));
					return MeshInfo{};
				}

				MagicaVoxel::ChunkHeader first;
				readFromStream(first, stream);
				if (!idMatches(first.id, "SIZE")) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Expected 'SIZE' chunk header in '", f, "'"));
					return MeshInfo{};
				}
				kengine_assert(first.childrenBytes == 0);
				kengine_assert(first.contentBytes == sizeof(MagicaVoxel::ChunkContent::Size));

				MagicaVoxel::ChunkContent::Size size;
				readFromStream(size, stream);

				PolyVox::RawVolume<PolyVoxComponent::VertexData> volume(PolyVox::Region{ { 0, 0, 0, }, { size.x, size.z, size.y } });

				MagicaVoxel::ChunkHeader voxelsHeader;
				readFromStream(voxelsHeader, stream);
				if (!idMatches(voxelsHeader.id, "XYZI")) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Expected 'XYZI' chunk header in '", f, "'"));
					return MeshInfo{};
				}

				MagicaVoxel::ChunkContent::RGBA rgba;

				MagicaVoxel::ChunkContent::XYZI xyzi;
				readFromStream(xyzi, stream);
				kengine_assert(voxelsHeader.contentBytes == sizeof(xyzi) + xyzi.numVoxels * sizeof(int));

				for (int i = 0; i < xyzi.numVoxels; ++i) {
					MagicaVoxel::ChunkContent::XYZI::Voxel voxel;
					readFromStream(voxel, stream);

					const auto & color = rgba.palette[voxel.colorIndex];
					const PolyVoxComponent::VertexData voxelValue{ { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f } };
					volume.setVoxel(voxel.x, voxel.z, voxel.y, voxelValue);
				}

				return { buildMesh(std::move(volume)), size };
			}

			static bool idMatches(const char * s1, const char * s2) noexcept {
				return strncmp(s1, s2, 4) == 0;
			}

			static void checkHeader(std::istream & s) noexcept {
				MagicaVoxel::FileHeader header;
				readFromStream(header, s);
				kengine_assert(idMatches(header.id, "VOX "));
				kengine_assert(header.versionNumber == 150);
			}

			static ModelDataComponent generateModelData(Entity & e, const MeshType & mesh) noexcept {
				ModelDataComponent modelData;
				modelData.meshes.push_back({});
				auto & meshData = modelData.meshes.back();
				meshData.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
				meshData.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
				meshData.indexType = putils::meta::type<putils_typeof(mesh)::IndexType>::index;

				modelData.free = release(e.id);
				modelData.init<MeshType::VertexType>();

				return modelData;
			}

			static void applyOffset(Entity & e, const MagicaVoxel::ChunkContent::Size & size) noexcept {
				if (e.has<TransformComponent>())
					return;

				auto & box = e.attach<TransformComponent>().boundingBox;
				box.position.x -= size.x / 2.f * box.size.x;
				box.position.z -= size.y / 2.f * box.size.z;
			}

			static void serialize(const char * f, const ModelDataComponent & modelData, const MagicaVoxel::ChunkContent::Size & size) noexcept {
				std::ofstream file(f, std::ofstream::binary | std::ofstream::trunc);
				if (!file) {
					kengine_assert_failed(putils::concat("[MagicaVoxel] Failed to serialize to '", f, "'"));
					return;
				}

				const auto write = [&](const auto & val) noexcept {
					file.write((const char *)&val, sizeof(val));
				};

				const auto & meshData = modelData.meshes.back();

				{
					write(meshData.vertices.nbElements);
					write(meshData.vertices.elementSize);
					const auto vertexBufferSize = meshData.vertices.nbElements * meshData.vertices.elementSize;
					file.write((const char *)meshData.vertices.data, vertexBufferSize);
				}

				{
					write(meshData.indices.nbElements);
					write(meshData.indices.elementSize);
					const auto indexBufferSize = meshData.indices.nbElements * meshData.indices.elementSize;
					file.write((const char *)meshData.indices.data, indexBufferSize);
				}

				write(meshData.indexType);

				write(size);
			}
		};

		return [](Entity & e) noexcept {
			impl::init(e);
		};
	}
}
