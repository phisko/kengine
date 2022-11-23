#include "MagicaVoxelSystem.hpp"

// stl
#include <filesystem>
#include <unordered_map>
#include <fstream>

// entt
#include <entt/entity/registry.hpp>

// polyvox
#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

// putils
#include "string.hpp"
#include "MagicaVoxel.hpp"

// kengine data
#include "data/InstanceComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/PolyVoxComponent.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> && volume) noexcept {
		const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encodedMesh);
		return mesh;
	}

	struct MagicaVoxelSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "MagicaVoxelSystem");

			_r = &r;

			r.on_construct<ModelComponent>().connect<[](entt::registry & r, entt::entity e) noexcept {
				loadModel(e);
			}>();
		}

		using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>{ {} }));
		struct MagicaVoxelModelComponent {
			MeshType mesh;
		};
		struct MeshInfo {
			MeshType mesh;
			MagicaVoxel::ChunkContent::Size size;
		};

		static void loadModel(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & f = _r->get<ModelComponent>(e).file.c_str();
			if (std::filesystem::path(f).extension() != ".vox")
				return;

			kengine_logf(*_r, Log, "MagicaVoxelSystem", "Loading model %zu for %s", e);
			const putils::string<256> binaryFile("%s.bin", f);

			if (std::filesystem::exists(binaryFile.c_str())) {
				kengine_log(*_r, Log, "MagicaVoxelSystem/loadModel", "Binary file exists, loading it");
				loadBinaryModel(e, binaryFile.c_str());
				return;
			}

			auto meshInfo = loadVoxModel(f);
			auto & mesh = _r->emplace<MagicaVoxelModelComponent>(e).mesh;
			mesh = std::move(meshInfo.mesh);

			auto modelData = generateModelData(e, mesh);
			serialize(binaryFile.c_str(), modelData, meshInfo.size);
			_r->emplace<ModelDataComponent>(e, std::move(modelData));

			applyOffset(e, meshInfo.size);
		}

		static void loadBinaryModel(entt::entity e, const char * binaryFile) noexcept {
			KENGINE_PROFILING_SCOPE;

			MagicaVoxel::ChunkContent::Size size;

			ModelDataComponent modelData;
			modelData.meshes.push_back({});
			unserialize(binaryFile, modelData.meshes.back(), size);
			modelData.free = release(e);
			modelData.init<MeshType::VertexType>();
			_r->emplace<ModelDataComponent>(e, std::move(modelData));

			if (_r->all_of<TransformComponent>(e)) {
				kengine_logf(*_r, Log, "MagicaVoxelSystem/loadModel", "%zu already has a TransformComponent. Mesh offset will not be applied", e);
				return;
			}
			kengine_log(*_r, Log, "MagicaVoxelSystem/loadModel", "Applying mesh offset");

			auto & box = _r->emplace<TransformComponent>(e).boundingBox;
			box.position.x -= size.x / 2.f * box.size.x;
			box.position.z -= size.y / 2.f * box.size.z;
		}

		static void unserialize(const char * f, ModelDataComponent::Mesh & meshData, MagicaVoxel::ChunkContent::Size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ifstream file(f, std::ofstream::binary);
			if (!file) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Failed to load '", f, "'");
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

		static ModelDataComponent::FreeFunc release(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			return [e] {
				const auto model = _r->try_get<MagicaVoxelModelComponent>(e);
				if (model) {
					model->mesh.clear();
					_r->remove<MagicaVoxelModelComponent>(e);
				}
				else { // Was unserialized and we (violently) `new`-ed the data buffers
					const auto & modelData = _r->get<ModelDataComponent>(e);
					delete[] (const char *)modelData.meshes[0].vertices.data;
					delete[] (const char *)modelData.meshes[0].indices.data;
				}
			};
		}

		static MeshInfo loadVoxModel(const char * f) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ifstream stream(f, std::ios::binary);
			if (!stream) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Failed to load '", f, "'");
				return MeshInfo{};
			}

			checkHeader(stream);

			MagicaVoxel::ChunkHeader main;
			readFromStream(main, stream);
			if (!idMatches(main.id, "MAIN")) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Expected 'MAIN' chunk header in '", f, "'");
				return MeshInfo{};
			}

			MagicaVoxel::ChunkHeader first;
			readFromStream(first, stream);
			if (!idMatches(first.id, "SIZE")) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Expected 'SIZE' chunk header in '", f, "'");
				return MeshInfo{};
			}
			kengine_assert(*_r, first.childrenBytes == 0);
			kengine_assert(*_r, first.contentBytes == sizeof(MagicaVoxel::ChunkContent::Size));

			MagicaVoxel::ChunkContent::Size size;
			readFromStream(size, stream);

			PolyVox::RawVolume<PolyVoxComponent::VertexData> volume(PolyVox::Region{ { 0, 0, 0, }, { size.x, size.z, size.y } });

			MagicaVoxel::ChunkHeader voxelsHeader;
			readFromStream(voxelsHeader, stream);
			if (!idMatches(voxelsHeader.id, "XYZI")) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Expected 'XYZI' chunk header in '", f, "'");
				return MeshInfo{};
			}

			MagicaVoxel::ChunkContent::RGBA rgba;

			MagicaVoxel::ChunkContent::XYZI xyzi;
			readFromStream(xyzi, stream);
			kengine_assert(*_r, voxelsHeader.contentBytes == sizeof(xyzi) + xyzi.numVoxels * sizeof(int));

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
			KENGINE_PROFILING_SCOPE;

			MagicaVoxel::FileHeader header;
			readFromStream(header, s);
			kengine_assert(*_r, idMatches(header.id, "VOX "));
			kengine_assert(*_r, header.versionNumber == 150);
		}

		static ModelDataComponent generateModelData(entt::entity e, const MeshType & mesh) noexcept {
			KENGINE_PROFILING_SCOPE;

			ModelDataComponent modelData;
			modelData.meshes.push_back({});
			auto & meshData = modelData.meshes.back();
			meshData.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
			meshData.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
			meshData.indexType = putils::meta::type<putils_typeof(mesh)::IndexType>::index;

			modelData.free = release(e);
			modelData.init<MeshType::VertexType>();

			return modelData;
		}

		static void applyOffset(entt::entity e, const MagicaVoxel::ChunkContent::Size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (_r->all_of<TransformComponent>(e)) {
				kengine_logf(*_r, Log, "MagicaVoxelSystem/loadModel", "%zu already has a TransformComponent. Mesh offset will not be applied", e);
				return;
			}
			kengine_log(*_r, Log, "MagicaVoxelSystem/loadModel", "Applying mesh offset");

			auto & box = _r->get_or_emplace<TransformComponent>(e).boundingBox;
			box.position.x -= size.x / 2.f * box.size.x;
			box.position.z -= size.y / 2.f * box.size.z;
		}

		static void serialize(const char * f, const ModelDataComponent & modelData, const MagicaVoxel::ChunkContent::Size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ofstream file(f, std::ofstream::binary | std::ofstream::trunc);
			if (!file) {
				kengine_assert_failed(*_r, "[MagicaVoxel] Failed to serialize to '", f, "'");
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

		template<typename T>
		static void readFromStream(T & header, std::istream & s) noexcept {
			s.read((char *)&header, sizeof(header));
			kengine_assert(*_r, s.gcount() == sizeof(header));
		}

		static inline entt::registry * _r;
	};

	void MagicaVoxelSystem(entt::registry & r) noexcept {
		MagicaVoxelSystem::init(r);
	}
}
