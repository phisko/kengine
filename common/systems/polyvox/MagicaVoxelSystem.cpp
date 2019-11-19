#include "MagicaVoxelSystem.hpp"

#include <filesystem>
#include <unordered_map>
#include <fstream>

#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

#include "EntityManager.hpp"

#include "components/ModelLoaderComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/PolyVoxComponent.hpp"
#include "components/DefaultShadowComponent.hpp"

#include "string.hpp"
#include "Export.hpp"
#include "file_extension.hpp"
#include "MagicaVoxel.hpp"

namespace kengine {
	namespace detailMagicaVoxel {
		static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> & volume) {
			const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
			const auto mesh = PolyVox::decodeMesh(encodedMesh);
			return mesh;
		}
	}

	static float toNormalizedColor(unsigned char color) {
		return (float)color / 255.f;
	}

	using MeshType = decltype(detailMagicaVoxel::buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>{ {} }));

	struct MagicaVoxelModelComponent {
		MeshType mesh;
	};

	static bool idMatches(const char * s1, const char * s2) {
		return strncmp(s1, s2, 4) == 0;
	}

	template<typename T>
	static void readFromStream(T & header, std::istream & s, unsigned int size) {
		s.read((char *)&header, size);
		assert(s.gcount() == size);
	}

	template<typename T>
	static void readFromStream(T & header, std::istream & s) {
		readFromStream(header, s, sizeof(header));
	}

	static void checkHeader(std::istream & s) {
		MagicaVoxel::FileHeader header;
		readFromStream(header, s);
		assert(idMatches(header.id, "VOX "));
		assert(header.versionNumber == 150);
	}

	static auto release(Entity::ID id, EntityManager & em) {
		return [&em, id] {
			auto & e = em.getEntity(id);
			if (e.has<MagicaVoxelModelComponent>()) {
				auto & model = e.get<MagicaVoxelModelComponent>();
				model.mesh.clear();
				e.detach<MagicaVoxelModelComponent>();
			}
			else { // Was unserialized and we (violently) `new`-ed the data buffers
				auto & meshData = e.get<ModelLoaderComponent::ModelData::MeshData>();
				delete[] meshData.vertices.data;
				delete[] meshData.indices.data;
			}
			e.detach<ModelLoaderComponent::ModelData::MeshData>();
		};
	}

	static void serialize(const char * f, const ModelLoaderComponent::ModelData::MeshData & meshData, const MagicaVoxel::ChunkContent::Size & size) {
		std::ofstream file(f, std::ofstream::binary | std::ofstream::trunc);
		assert(f);

		const auto write = [&](const auto & val) {
			file.write((const char *)&val, sizeof(val));
		};

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

	static void unserialize(const char * f, ModelLoaderComponent::ModelData::MeshData & meshData, MagicaVoxel::ChunkContent::Size & size) {
		std::ifstream file(f, std::ofstream::binary);
		assert(f);

		const auto parse = [&](auto & val) {
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

	static auto extractData(Entity::ID id, EntityManager & em) {
		return [&em, id] {
			auto & e = em.getEntity(id);

			ModelLoaderComponent::ModelData ret;
			ret.meshes.push_back(e.get<ModelLoaderComponent::ModelData::MeshData>());
			return ret;
		};
	}

	static bool loadVoxFile(Entity & e) {
		const auto & f = e.get<ModelComponent>().file.c_str();
		if (putils::file_extension(f) != "vox")
			return false;

		const putils::string<256> binaryFile("%s.bin", f);

		if (std::filesystem::exists(binaryFile.c_str())) {
			MagicaVoxel::ChunkContent::Size size;
			unserialize(binaryFile.c_str(), e.attach<ModelLoaderComponent::ModelData::MeshData>(), size);

			auto & box = e.get<ModelComponent>().boundingBox;
			box.position.x += size.x / 2.f * box.size.x;
			box.position.z += size.y / 2.f * box.size.z;

			return true;
		}

#ifndef KENGINE_NDEBUG
		std::cout << putils::termcolor::green << "[MagicaVoxel] Loading " << putils::termcolor::cyan << f << putils::termcolor::green << "..." << putils::termcolor::reset;
#endif
		std::ifstream stream(f, std::ios::binary);
		assert(stream);
		checkHeader(stream);

		MagicaVoxel::ChunkHeader main;
		readFromStream(main, stream);
		assert(idMatches(main.id, "MAIN"));

		MagicaVoxel::ChunkHeader first;
		readFromStream(first, stream);
		assert(idMatches(first.id, "SIZE")); // "PACK" not yet supported
		assert(first.childrenBytes == 0);
		assert(first.contentBytes == sizeof(MagicaVoxel::ChunkContent::Size));

		MagicaVoxel::ChunkContent::Size size;
		readFromStream(size, stream);

		PolyVox::RawVolume<PolyVoxComponent::VertexData> volume(PolyVox::Region{ { 0, 0, 0, }, { size.x, size.z, size.y } });

		MagicaVoxel::ChunkHeader voxelsHeader;
		readFromStream(voxelsHeader, stream);
		assert(idMatches(voxelsHeader.id, "XYZI"));

		MagicaVoxel::ChunkContent::RGBA rgba;

		MagicaVoxel::ChunkContent::XYZI xyzi;
		readFromStream(xyzi, stream);
		assert(voxelsHeader.contentBytes == sizeof(xyzi) + xyzi.numVoxels * sizeof(int));

		for (int i = 0; i < xyzi.numVoxels; ++i) {
			MagicaVoxel::ChunkContent::XYZI::Voxel voxel;
			readFromStream(voxel, stream);

			const auto & color = rgba.palette[voxel.colorIndex];
			const PolyVoxComponent::VertexData voxelValue{ { toNormalizedColor(color.r), toNormalizedColor(color.g), toNormalizedColor(color.b) } };
			volume.setVoxel(voxel.x, voxel.z, voxel.y, voxelValue);
		}

		auto & mesh = e.attach<MagicaVoxelModelComponent>().mesh;
		mesh = detailMagicaVoxel::buildMesh(volume);

		auto & meshData = e.attach<ModelLoaderComponent::ModelData::MeshData>();
		meshData.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
		meshData.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
		meshData.indexType = sizeof(putils_typeof(mesh)::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		serialize(binaryFile, meshData, size);

		auto & box = e.get<ModelComponent>().boundingBox;
		box.position.x += size.x / 2.f * box.size.x;
		box.position.z += size.y / 2.f * box.size.z;

#ifndef KENGINE_NDEBUG
		std::cout << putils::termcolor::green << " Done.\n" << putils::termcolor::reset;
#endif
		return true;
	}

	MagicaVoxelSystem::MagicaVoxelSystem(EntityManager & em) : System(em), _em(em) {
	}

	void MagicaVoxelSystem::loadModel(Entity & e) {
		if (!loadVoxFile(e))
			return;

		e += ModelLoaderComponent{
			extractData(e.id, _em),
			release(e.id, _em),
			[]() { putils::gl::setPolyvoxVertexType<MeshType::VertexType>(); }
		};
	}

	void MagicaVoxelSystem::setModel(Entity & e) {
		auto & graphics = e.get<GraphicsComponent>();

		if (putils::file_extension(graphics.appearance.c_str()) != "vox")
			return;

		e += PolyVoxObjectComponent{};
		e += DefaultShadowComponent{};

		graphics.model = Entity::INVALID_ID;
		for (const auto &[e, model] : _em.getEntities<ModelComponent>())
			if (model.file == graphics.appearance) {
				graphics.model = e.id;
				return;
			}

		_em += [&](Entity & e) {
			e += ModelComponent{ graphics.appearance.c_str() };
			graphics.model = e.id;
		};
	}


	void MagicaVoxelSystem::handle(packets::RegisterEntity p) {
		if (p.e.has<ModelComponent>())
			loadModel(p.e);
		else if (p.e.has<GraphicsComponent>())
			setModel(p.e);
	}
}
