#include "MagicaVoxelSystem.hpp"

#include <unordered_map>

#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

#include "EntityManager.hpp"

#include "components/ModelLoaderComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/PolyVoxComponent.hpp"

#include "string.hpp"
#include "Export.hpp"
#include "file_extension.hpp"

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

	struct MeshEntity {
		MeshType mesh;
		float pitch;
		float yaw;
		putils::Vector3f offset;
		kengine::Entity::ID id;
	};

	static inline std::unordered_map<putils::string<64>, MeshEntity> _meshes;

	// based on https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt

	struct FileHeader {
		char id[4]; // Must be "VOX "
		int versionNumber; // Must be 150
	};
	static_assert(sizeof(FileHeader) == 8);

	struct ChunkHeader {
		char id[4];
		int contentBytes;
		int childrenBytes;
	};
	static_assert(sizeof(ChunkHeader) == 12);

	struct ChunkContent {
		struct Pack {
			int models;
		};
		static_assert(sizeof(Pack) == 4);

		struct Size {
			int x;
			int y;
			int z;
		};
		static_assert(sizeof(Size) == 12);

		struct XYZI {
			union Voxel {
				int n;
				struct {
					unsigned char x;
					unsigned char y;
					unsigned char z;
					unsigned char colorIndex;
				};
			};
			static_assert(sizeof(Voxel) == sizeof(int));

			int numVoxels;
			// Voxel voxels[0];
		};
		static_assert(sizeof(XYZI) == 4);

		struct RGBA {
			union Color {
				unsigned int n;
				struct {
					unsigned char r;
					unsigned char g;
					unsigned char b;
					unsigned char a;
				};
			};
			static_assert(sizeof(Color) == sizeof(int));

			Color palette[256] = {
					0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
					0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
					0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
					0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
					0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
					0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
					0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
					0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
					0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
					0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
					0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
					0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
					0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
					0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
					0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
					0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
			};
			// <NOTICE>
			// color[0 - 254] are mapped to palette index[1 - 255], e.g :
			// for (int i = 0; i <= 254; i++) {
			//	 palette[i + 1] = ReadRGBA();
			// }
		};
		static_assert(sizeof(RGBA) == 256 * 4);

		struct Matt {
			int id;

			enum MaterialType {
				Diffuse, Metal, Glass, Emissive
			};
			MaterialType materialType;

			float materialWeight;
			// diffuse  : 1.0
			// metal : (0.0 - 1.0] : blend between metal and diffuse material
			// glass : (0.0 - 1.0] : blend between glass and diffuse material
			// emissive : (0.0 - 1.0] : self - illuminated material

			int propertyBits; // set if value is saved in next section
			// bit(0) : Plastic
			// bit(1) : Roughness
			// bit(2) : Specular
			// bit(3) : IOR
			// bit(4) : Attenuation
			// bit(5) : Power
			// bit(6) : Glow
			// bit(7) : isTotalPower(*no value)

			// float normalizedPropertyValue[0];
		};
		static_assert(sizeof(Matt) == 16);
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
		FileHeader header;
		readFromStream(header, s);
		assert(idMatches(header.id, "VOX "));
		assert(header.versionNumber == 150);
	}

	static std::function<ModelLoaderComponent::ModelData()> loadVoxFile(const char * file, MeshType & mesh, float pitch = 0.f, float yaw = 0.f, const putils::Vector3f & offsetFromCentre = {}) {
		const putils::string<64> f(file);

		return [&mesh, f, pitch, yaw, offsetFromCentre] {
			std::ifstream stream(f.c_str(), std::ios::binary);
			assert(stream);
			checkHeader(stream);

			ChunkHeader main;
			readFromStream(main, stream);
			assert(idMatches(main.id, "MAIN"));

			ChunkHeader first;
			readFromStream(first, stream);
			assert(idMatches(first.id, "SIZE")); // "PACK" not yet supported
			assert(first.childrenBytes == 0);
			assert(first.contentBytes == sizeof(ChunkContent::Size));

			ChunkContent::Size size;
			readFromStream(size, stream);

			PolyVox::RawVolume<PolyVoxComponent::VertexData> volume(PolyVox::Region{ { 0, 0, 0, }, { size.x, size.y, size.z } });

			ChunkHeader voxelsHeader;
			readFromStream(voxelsHeader, stream);
			assert(idMatches(voxelsHeader.id, "XYZI"));

			ChunkContent::RGBA rgba;

			ChunkContent::XYZI xyzi;
			readFromStream(xyzi, stream);
			assert(voxelsHeader.contentBytes == sizeof(xyzi) + xyzi.numVoxels * sizeof(int));

			for (int i = 0; i < xyzi.numVoxels; ++i) {
				ChunkContent::XYZI::Voxel voxel;
				readFromStream(voxel, stream);

				const auto & color = rgba.palette[voxel.colorIndex];
				const PolyVoxComponent::VertexData voxelValue{ { toNormalizedColor(color.r), toNormalizedColor(color.g), toNormalizedColor(color.b) } };
				volume.setVoxel(voxel.x, voxel.y, voxel.z, voxelValue);
			}

			mesh = detailMagicaVoxel::buildMesh(volume);

			kengine::ModelLoaderComponent::ModelData ret;

			kengine::ModelLoaderComponent::ModelData::MeshData meshData;
			meshData.vertices = { mesh.getNoOfVertices(), sizeof(pmeta_typeof(mesh)::VertexType), mesh.getRawVertexData() };
			meshData.indices = { mesh.getNoOfIndices(), sizeof(pmeta_typeof(mesh)::IndexType), mesh.getRawIndexData() };
			meshData.indexType = sizeof(pmeta_typeof(mesh)::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			ret.meshes.push_back(meshData);

			ret.pitch = pitch;
			ret.yaw = yaw;
			ret.offsetToCentre = offsetFromCentre;

			return ret;
		};
	}

	MagicaVoxelSystem::MagicaVoxelSystem(kengine::EntityManager & em) : System(em), _em(em) {
		onLoad("");
	}

	void MagicaVoxelSystem::onLoad(const char *) noexcept {
		for (auto & p : _meshes) {
			const auto oldId = p.second.id;

			_em += [&p](kengine::Entity & e) {
				e += kengine::ModelLoaderComponent{
					loadVoxFile(p.first.c_str(), p.second.mesh, p.second.pitch, p.second.yaw, p.second.offset),
					[]() { putils::gl::setPolyvoxVertexType<MeshType::VertexType>(); }
				};
				p.second.id = e.id;
			};
		}

		for (const auto &[e, graphics, model] : _em.getEntities<kengine::GraphicsComponent, kengine::ModelComponent>()) {
			const auto file = graphics.getLayer("main").appearance;
			model.modelInfo = _meshes[file].id;
		}
	}

	void MagicaVoxelSystem::handle(kengine::packets::RegisterEntity p) {
		if (!p.e.has<kengine::GraphicsComponent>())
			return;

		auto & graphics = p.e.get<kengine::GraphicsComponent>();
		const auto & layer = graphics.getLayer("main");
		const auto & file = layer.appearance;
		if (strcmp(putils::file_extension(file.c_str()), "vox") != 0)
			return;

		p.e += PolyVoxModelComponent{};

		const auto it = _meshes.find(file);
		if (it != _meshes.end()) {
			p.e += kengine::ModelComponent{ it->second.id };
			return;
		}

		auto & meshData = _meshes[file];
		meshData.pitch = layer.pitch;
		meshData.yaw = layer.yaw;
		_em += [&meshData, file](kengine::Entity & e) {
			e += kengine::ModelLoaderComponent{
				loadVoxFile(file.c_str(), meshData.mesh, meshData.pitch, meshData.yaw),
				[]() { putils::gl::setPolyvoxVertexType<MeshType::VertexType>(); }
			};
			meshData.id = e.id;
		};

		p.e += kengine::ModelComponent{ meshData.id };
	}
}
