#include "MagicaVoxelSystem.hpp"

#include <filesystem>
#include <unordered_map>
#include <fstream>

#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

#include "EntityManager.hpp"

#include "data/ModelDataComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/PolyVoxComponent.hpp"
#include "data/DefaultShadowComponent.hpp"

#include "functions/OnEntityCreated.hpp"

#include "string.hpp"
#include "Export.hpp"
#include "file_extension.hpp"
#include "MagicaVoxel.hpp"

namespace kengine {
	static EntityManager * g_em;

	// declarations
	static void onEntityCreated(Entity & e);
	//
	EntityCreator * MagicaVoxelSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
		};
	}

	// declarations
	static void loadModel(Entity & e);
	static void setModel(Entity & e);
	//
	static void onEntityCreated(Entity & e) {
		if (e.has<ModelComponent>())
			loadModel(e);
		else if (e.has<GraphicsComponent>())
			setModel(e);
	}

	static void setModel(Entity & e) {
		auto & graphics = e.get<GraphicsComponent>();

		if (putils::file_extension(graphics.appearance.c_str()) != "vox")
			return;

		e += PolyVoxObjectComponent{};
		e += DefaultShadowComponent{};

		graphics.model = Entity::INVALID_ID;
		for (const auto &[e, model] : g_em->getEntities<ModelComponent>())
			if (model.file == graphics.appearance) {
				graphics.model = e.id;
				return;
			}

		*g_em += [&](Entity & e) {
			e += ModelComponent{ graphics.appearance.c_str() };
			graphics.model = e.id;
		};
	}

	namespace detailMagicaVoxel {
		static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> & volume) {
			const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
			const auto mesh = PolyVox::decodeMesh(encodedMesh);
			return mesh;
		}
	}
	using MeshType = decltype(detailMagicaVoxel::buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>{ {} }));
	struct MagicaVoxelModelComponent {
		MeshType mesh;
	};
	struct MeshInfo {
		MeshType mesh;
		MagicaVoxel::ChunkContent::Size size;
	};

	// declarations
	static void loadBinaryModel(Entity & e, const char * binaryFile);
	static MeshInfo loadVoxModel(const char * f);
	static ModelDataComponent generateModelData(Entity & e, const MeshType & mesh);
	static void serialize(const char * f, const ModelDataComponent & modelData, const MagicaVoxel::ChunkContent::Size & size);
	static void applyOffset(Entity & e, const MagicaVoxel::ChunkContent::Size & size);
	//
	static void loadModel(Entity & e) {
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
		const auto meshInfo = loadVoxModel(f);
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

	// declarations
	static void checkHeader(std::istream & s);
	static bool idMatches(const char * s1, const char * s2);
	template<typename T>
	static void readFromStream(T & header, std::istream & s) {
		s.read((char *)&header, sizeof(header));
		assert(s.gcount() == sizeof(header));
	}
	//
	static MeshInfo loadVoxModel(const char * f) {
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
			const PolyVoxComponent::VertexData voxelValue{ { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f } };
			volume.setVoxel(voxel.x, voxel.z, voxel.y, voxelValue);
		}

		return { detailMagicaVoxel::buildMesh(volume), size };
	}

	static bool idMatches(const char * s1, const char * s2) {
		return strncmp(s1, s2, 4) == 0;
	}

	static void checkHeader(std::istream & s) {
		MagicaVoxel::FileHeader header;
		readFromStream(header, s);
		assert(idMatches(header.id, "VOX "));
		assert(header.versionNumber == 150);
	}

	// declarations
	static ModelDataComponent::FreeFunc release(Entity::ID id, EntityManager & em);
	//
	static ModelDataComponent generateModelData(Entity & e, const MeshType & mesh) {
		ModelDataComponent modelData;
		modelData.meshes.push_back({});
		auto & meshData = modelData.meshes.back();
		meshData.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
		meshData.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
		meshData.indexType = sizeof(putils_typeof(mesh)::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		modelData.free = release(e.id, *g_em);
		modelData.init<MeshType::VertexType>();

		return modelData;
	}

	static ModelDataComponent::FreeFunc release(Entity::ID id, EntityManager & em) {
		return [&em, id] {
			auto & e = em.getEntity(id);
			if (e.has<MagicaVoxelModelComponent>()) {
				auto & model = e.get<MagicaVoxelModelComponent>();
				model.mesh.clear();
				e.detach<MagicaVoxelModelComponent>();
			}
			else { // Was unserialized and we (violently) `new`-ed the data buffers
				const auto & modelData = e.get<ModelDataComponent>();
				delete[] modelData.meshes[0].vertices.data;
				delete[] modelData.meshes[0].indices.data;
			}
		};
	}


	static void applyOffset(Entity & e, const MagicaVoxel::ChunkContent::Size & size) {
		auto & box = e.get<ModelComponent>().boundingBox;
		box.position.x += size.x / 2.f * box.size.x;
		box.position.z += size.y / 2.f * box.size.z;
	}

	static void serialize(const char * f, const ModelDataComponent & modelData, const MagicaVoxel::ChunkContent::Size & size) {
		std::ofstream file(f, std::ofstream::binary | std::ofstream::trunc);
		assert(f);

		const auto write = [&](const auto & val) {
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

	// declarations
	static void unserialize(const char * f, ModelDataComponent::Mesh & meshData, MagicaVoxel::ChunkContent::Size & size);
	//
	static void loadBinaryModel(Entity & e, const char * binaryFile) {
		MagicaVoxel::ChunkContent::Size size;

		ModelDataComponent modelData;
		modelData.meshes.push_back({});
		unserialize(binaryFile, modelData.meshes.back(), size);
		modelData.free = release(e.id, *g_em);
		modelData.init<MeshType::VertexType>();
		e += std::move(modelData);

		auto & box = e.get<ModelComponent>().boundingBox;
		box.position.x += size.x / 2.f * box.size.x;
		box.position.z += size.y / 2.f * box.size.z;
	}

	static void unserialize(const char * f, ModelDataComponent::Mesh & meshData, MagicaVoxel::ChunkContent::Size & size) {
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
}
