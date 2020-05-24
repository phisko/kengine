#include <filesystem>
#include <fstream>

#include "EntityManager.hpp"

#include "RecastSystem.hpp"
#include "RecastComponent.hpp"
#include "RecastDebugShader.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/NavMeshComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "functions/Execute.hpp"

#include "helpers/assertHelper.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"

#include "angle.hpp"
#include "with.hpp"

namespace Flags {
	enum {
		Walk = 1,
	};
}

namespace kengine {
	static EntityManager * g_em;

	// declarations
	static void execute(float deltaTime);
	//
	EntityCreator * RecastSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::Execute{ execute };
			e += makeGBufferShaderComponent<RecastDebugShader>(*g_em);
		};
	}

	// declarations
	static void buildNavMeshes();
	//
	static void execute(float deltaTime) {
		buildNavMeshes();
	}

	// declarations
	static void createRecastMesh(const char * file, Entity & model, NavMeshComponent & navMesh, const ModelDataComponent & modelData);
	//
	static void buildNavMeshes() {
		static const auto buildRecastComponent = [](auto && entities) {
			for (auto & [e, model, modelData, navMesh, _] : entities) {
				g_em->runTask([&] {
					kengine_assert(*g_em, navMesh.vertsPerPoly <= DT_VERTS_PER_POLYGON);
					createRecastMesh(model.file, e, navMesh, modelData);
					if constexpr (std::is_same<RebuildNavMeshComponent, putils_typeof(_)>())
						e.detach<RebuildNavMeshComponent>();
				});
			}
			g_em->completeTasks();
		};

		buildRecastComponent(g_em->getEntities<ModelComponent, ModelDataComponent, NavMeshComponent, no<RecastComponent>>());
		buildRecastComponent(g_em->getEntities<ModelComponent, ModelDataComponent, NavMeshComponent, RebuildNavMeshComponent>());
	}

	// declarations
	using HeightfieldPtr = UniquePtr<rcHeightfield, rcFreeHeightField>;
	using CompactHeightfieldPtr = UniquePtr<rcCompactHeightfield, rcFreeCompactHeightfield>;
	using ContourSetPtr = UniquePtr<rcContourSet, rcFreeContourSet>;
	using PolyMeshPtr = UniquePtr<rcPolyMesh, rcFreePolyMesh>;
	using PolyMeshDetailPtr = UniquePtr<rcPolyMeshDetail, rcFreePolyMeshDetail>;

	struct NavMeshData {
		unsigned char * data = nullptr;
		int size = 0;
	};

	static NavMeshData loadBinaryFile(const char * binaryFile, const NavMeshComponent & navMesh);
	static void saveBinaryFile(const char * binaryFile, const NavMeshData & data, const NavMeshComponent & navMesh);
	static NavMeshData createNavMeshData(const NavMeshComponent & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	static NavMeshPtr createNavMesh(const NavMeshData & data);
	static NavMeshQueryPtr createNavMeshQuery(const NavMeshComponent & params, const dtNavMesh & navMesh);
	//
	static void createRecastMesh(const char * file, Entity & e, NavMeshComponent & navMesh, const ModelDataComponent & modelData) {
		NavMeshData data;

		const putils::string<4096> binaryFile("%s.nav", file);
		bool mustSave = false;
		data = loadBinaryFile(binaryFile, navMesh);
		if (data.data == nullptr) {
			data = createNavMeshData(navMesh, modelData, modelData.meshes[navMesh.concernedMesh]);
			if (data.data == nullptr)
				return;
			mustSave = true;
		}

		auto & recast = e.attach<RecastComponent>();
		recast.navMesh = createNavMesh(data);
		if (recast.navMesh == nullptr) {
			dtFree(data.data);
			return;
		}

		recast.navMeshQuery = createNavMeshQuery(navMesh, *recast.navMesh);
		if (recast.navMeshQuery == nullptr) {
			dtFree(data.data);
			return;
		}

		if (mustSave)
			saveBinaryFile(binaryFile, data, navMesh);

		const auto & model = e.get<ModelComponent>();
		navMesh.getPath = [&](const Entity & e, const putils::Point3f & startWorldSpace, const putils::Point3f & endWorldSpace) {
			const auto mat = shaderHelper::getModelMatrix(model, e.get<TransformComponent>());

			float start[3];
			{
				auto tmp = glm::vec4(shaderHelper::toVec(startWorldSpace), 1.f);
				tmp = glm::inverse(mat) * tmp;
				start[0] = tmp.x;
				start[1] = tmp.y;
				start[2] = tmp.z;
			}

			float end[3];
			{
				auto tmp = glm::vec4(shaderHelper::toVec(endWorldSpace), 1.f);
				tmp = glm::inverse(mat) * tmp;
				end[0] = tmp.x;
				end[1] = tmp.y;
				end[2] = tmp.z;
			}


			NavMeshComponent::Path ret;

			const float extents[3] = { navMesh.characterRadius * 2.f, navMesh.characterHeight, navMesh.characterRadius * 2.f };

			dtPolyRef startRef;
			float startPt[3];
			auto status = recast.navMeshQuery->findNearestPoly(start, extents, nullptr, &startRef, startPt);
			if (dtStatusFailed(status) || startRef == 0) {
				kengine_assert_failed(*g_em, "[Recast] Failed to find nearest poly");
				return ret;
			}

			dtPolyRef endRef;
			float endPt[3];
			status = recast.navMeshQuery->findNearestPoly(end, extents, nullptr, &endRef, endPt);
			if (dtStatusFailed(status) || endRef == 0) {
				kengine_assert_failed(*g_em, "[Recast] Failed to find nearest poly");
				return ret;
			}

			dtPolyRef path[KENGINE_NAVMESH_MAX_PATH_LENGTH];
			int pathCount = 0;
			status = recast.navMeshQuery->findPath(startRef, endRef, startPt, endPt, nullptr, path, &pathCount, lengthof(path));
			if (dtStatusFailed(status)) {
				kengine_assert_failed(*g_em, "[Recast] Failed to find path");
				return ret;
			}

			static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
			ret.resize(ret.capacity());

			int straightPathCount = 0;
			status = recast.navMeshQuery->findStraightPath(startPt, endPt, path, pathCount, ret[0].raw, nullptr, nullptr, &straightPathCount, ret.capacity());
			if (dtStatusFailed(status)) {
				kengine_assert_failed(*g_em, "[Recast] Failed to find straight path");
				return ret;
			}
			ret.resize(straightPathCount);

			return ret;
		};
	}

	static NavMeshData loadBinaryFile(const char * binaryFile, const NavMeshComponent & navMesh) {
		NavMeshData data;

		std::ifstream f(binaryFile, std::ifstream::binary);
		if (!f)
			return data;

		NavMeshComponent header;
		f.read((char *)&header, sizeof(header));
		if (std::memcmp(&header, &navMesh, sizeof(header)))
			return data; // Different parameters

		f.read((char *)&data.size, sizeof(data.size));
		data.data = (unsigned char *)dtAlloc(data.size, dtAllocHint::DT_ALLOC_PERM);
		f.read((char *)data.data, data.size);

		return data;
	}

	static void saveBinaryFile(const char * binaryFile, const NavMeshData & data, const NavMeshComponent & navMesh) {
		std::ofstream f(binaryFile, std::ofstream::trunc | std::ofstream::binary);
		f.write((const char *)&navMesh, sizeof(navMesh));
		f.write((const char *)&data.size, sizeof(data.size));
		f.write((const char *)data.data, data.size);
	}

	// declarations
	static std::unique_ptr<float[]> getVertices(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	static rcConfig getConfig(const NavMeshComponent & navMesh, const ModelDataComponent::Mesh & meshData, const float * vertices);
	static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData, const float * vertices);
	static CompactHeightfieldPtr createCompactHeightField(rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField);
	static ContourSetPtr createContourSet(rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf);
	static PolyMeshPtr createPolyMesh(rcContext & ctx, const rcConfig & cfg, rcContourSet & contourSet);
	static PolyMeshDetailPtr createPolyMeshDetail(rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcCompactHeightfield & chf);
	//
	static NavMeshData createNavMeshData(const NavMeshComponent & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) {
		NavMeshData ret;

		const auto vertices = getVertices(modelData, meshData);

		const auto cfg = getConfig(navMesh, meshData, vertices.get());
		if (cfg.width == 0 || cfg.height == 0) {
			kengine_assert_failed(*g_em, "[Recast] Mesh was 0 height or width?");
			return ret;
		}

		rcContext ctx;

		const auto heightField = createHeightField(ctx, cfg, meshData, vertices.get());
		if (heightField == nullptr)
			return ret;

		rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *heightField);
		rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField);
		rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *heightField);

		const auto compactHeightField = createCompactHeightField(ctx, cfg, *heightField);
		if (compactHeightField == nullptr)
			return ret;

		const auto contourSet = createContourSet(ctx, cfg, *compactHeightField);
		if (contourSet == nullptr)
			return ret;

		const auto polyMesh = createPolyMesh(ctx, cfg, *contourSet);
		if (polyMesh == nullptr)
			return ret;

		const auto polyMeshDetail = createPolyMeshDetail(ctx, cfg, *polyMesh, *compactHeightField);
		if (polyMeshDetail == nullptr)
			return ret;
	
		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		{ putils_with(*polyMesh) {
			params.verts = _.verts;
			params.vertCount = _.nverts;
			params.polys = _.polys;
			params.polyAreas = _.areas;
			params.polyFlags = _.flags;
			params.polyCount = _.npolys;
			params.nvp = _.nvp;
		} }

		{ putils_with(*polyMeshDetail) {
			params.detailMeshes = _.meshes;
			params.detailVerts = _.verts;
			params.detailVertsCount = _.nverts;
			params.detailTris = _.tris;
			params.detailTriCount = _.ntris;
		} }

		params.walkableHeight = (float)cfg.walkableHeight;
		params.walkableClimb = (float)cfg.walkableClimb;
		params.walkableRadius = (float)cfg.walkableRadius;
		rcVcopy(params.bmin, cfg.bmin);
		rcVcopy(params.bmax, cfg.bmax);
		params.cs = cfg.cs;
		params.ch = cfg.ch;

		if (!dtCreateNavMeshData(&params, &ret.data, &ret.size))
			kengine_assert_failed(*g_em, "[Recast] Failed to create Detour navmesh data");
		return ret;
	}

	// declarations
	const std::ptrdiff_t getVertexPositionOffset(const ModelDataComponent & modelData);
	const float * getVertexPosition(const void * vertices, size_t index, size_t vertexSize, std::ptrdiff_t positionOffset);
	//
	std::unique_ptr<float[]> getVertices(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) {
		const auto vertexSize = modelData.getVertexSize();
		const auto positionOffset = getVertexPositionOffset(modelData);
		if (positionOffset == -1)
			return nullptr;

		auto vertices = std::unique_ptr<float[]>(new float[meshData.vertices.nbElements * 3]);

		for (size_t vertex = 0; vertex < meshData.vertices.nbElements; ++vertex) {
			const auto pos = getVertexPosition(meshData.vertices.data, vertex, vertexSize, positionOffset);
			for (size_t i = 0; i < 3; ++i)
				vertices[vertex * 3 + i] = pos[i];
		}

		return vertices;
	}

	const std::ptrdiff_t getVertexPositionOffset(const ModelDataComponent & modelData) {
		static const char * potentialNames[] = { "pos", "position" };

		for (const auto name : potentialNames) {
			const auto offset = modelData.getVertexAttributeOffset(name);
			if (offset >= 0)
				return offset;
		}

		kengine_assert_failed(*g_em, "[Recast] Could not find vertex position");
		return -1;
	}

	const float * getVertexPosition(const void * vertices, size_t index, size_t vertexSize, std::ptrdiff_t positionOffset) {
		const auto vertex = (const char *)vertices + index * vertexSize;
		return (const float *)(vertex + positionOffset);
	}

	static rcConfig getConfig(const NavMeshComponent & navMesh, const ModelDataComponent::Mesh & meshData, const float * vertices) {
		rcConfig cfg;
		memset(&cfg, 0, sizeof(cfg));

		{ putils_with(navMesh) {
			cfg.cs = _.cellSize;
			kengine_assert(*g_em, cfg.cs > 0);

			cfg.ch = _.cellHeight;
			kengine_assert(*g_em, cfg.ch > 0);

			cfg.walkableSlopeAngle = putils::toDegrees(_.walkableSlope);
			kengine_assert(*g_em, cfg.walkableSlopeAngle > 0.f && cfg.walkableSlopeAngle <= 90.f);

			cfg.walkableHeight = (int)ceilf(_.characterHeight / _.cellHeight);
			kengine_assert(*g_em, cfg.walkableHeight >= 3);

			cfg.walkableClimb = (int)floorf(_.characterClimb / _.cellHeight);
			kengine_assert(*g_em, cfg.walkableClimb >= 0);

			cfg.walkableRadius = (int)ceilf(_.characterRadius / _.cellSize);
			kengine_assert(*g_em, cfg.walkableRadius >= 0);

			cfg.maxEdgeLen = (int)(_.maxEdgeLength / _.cellSize);
			kengine_assert(*g_em, cfg.maxEdgeLen >= 0);

			cfg.maxSimplificationError = _.maxSimplificationError;
			kengine_assert(*g_em, cfg.maxSimplificationError >= 0);

			cfg.minRegionArea = (int)rcSqr(_.minRegionArea);
			kengine_assert(*g_em, cfg.minRegionArea >= 0);

			cfg.mergeRegionArea = (int)rcSqr(_.mergeRegionArea);
			kengine_assert(*g_em, cfg.mergeRegionArea >= 0);

			cfg.maxVertsPerPoly = _.vertsPerPoly;
			kengine_assert(*g_em, cfg.maxVertsPerPoly >= 3);

			cfg.detailSampleDist = _.detailSampleDist;
			kengine_assert(*g_em, cfg.detailSampleDist == 0.f || cfg.detailSampleDist >= .9f);

			cfg.detailSampleMaxError = _.detailSampleMaxError;
			kengine_assert(*g_em, cfg.detailSampleMaxError >= 0.f);
		} }

		rcCalcBounds(vertices, (int)meshData.vertices.nbElements, cfg.bmin, cfg.bmax);
		rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

		return cfg;
	}

	static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData, const float * vertices) {
		HeightfieldPtr heightField{ rcAllocHeightfield() };

		if (heightField == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate height field");
			return nullptr;
		}

		if (!rcCreateHeightfield(&ctx, *heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to create height field");
			return nullptr;
		}

		const auto nbTriangles = meshData.indices.nbElements / 3; // I think?
		const auto triangleAreas = new unsigned char[nbTriangles];
		memset(triangleAreas, 0, nbTriangles);

		int * indices = (int *)meshData.indices.data;
		bool mustDeleteIndices = false;
		if (meshData.indexType == GL_UNSIGNED_INT) {
			indices = new int[meshData.indices.nbElements];
			mustDeleteIndices = true;
			const auto unsignedIndices = (const unsigned int *)meshData.indices.data;
			for (int i = 0; i < meshData.indices.nbElements; ++i)
				indices[i] = (int)unsignedIndices[i];
		}

		rcMarkWalkableTriangles(&ctx, cfg.walkableSlopeAngle,
			vertices, (int)meshData.vertices.nbElements,
			indices, (int)nbTriangles,
			triangleAreas);

		if (!rcRasterizeTriangles(&ctx, vertices, (int)meshData.vertices.nbElements, indices, triangleAreas, (int)nbTriangles, *heightField, cfg.walkableClimb)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to rasterize triangles");
			delete[] triangleAreas;
			return nullptr;
		}

		if (mustDeleteIndices)
			delete[] indices;

		delete[] triangleAreas;

		return heightField;
	}

	static CompactHeightfieldPtr createCompactHeightField(rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField) {
		CompactHeightfieldPtr compactHeightField{ rcAllocCompactHeightfield() };

		if (compactHeightField == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate compact height field");
			return nullptr;
		}

		if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, heightField, *compactHeightField)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build compact height field");
			return nullptr;
		}

		if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *compactHeightField)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to erode walkable area");
			return nullptr;
		}
		
		// Classic recast positiong. For others, see https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_SoloMesh.cpp
		if (!rcBuildDistanceField(&ctx, *compactHeightField)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build distance field");
			return nullptr;
		}

		if (!rcBuildRegions(&ctx, *compactHeightField, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build regions");
			return nullptr;
		}

		return compactHeightField;
	}

	static ContourSetPtr createContourSet(rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf) {
		ContourSetPtr contourSet{ rcAllocContourSet() };

		if (contourSet == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate contour set");
			return nullptr;
		}

		if (!rcBuildContours(&ctx, chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build contours");
			return nullptr;
		}

		return contourSet;
	}

	static PolyMeshPtr createPolyMesh(rcContext & ctx, const rcConfig & cfg, rcContourSet & contourSet) {
		PolyMeshPtr polyMesh{ rcAllocPolyMesh() };

		if (polyMesh == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate poly mesh");
			return nullptr;
		}
		
		if (!rcBuildPolyMesh(&ctx, contourSet, cfg.maxVertsPerPoly, *polyMesh)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build poly mesh");
			return nullptr;
		}

		return polyMesh;
	}

	static PolyMeshDetailPtr createPolyMeshDetail(rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcCompactHeightfield & chf) {
		PolyMeshDetailPtr polyMeshDetail{ rcAllocPolyMeshDetail() };
		if (polyMeshDetail == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate poly mesh detail");
			return nullptr;
		}

		if (!rcBuildPolyMeshDetail(&ctx, polyMesh, chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *polyMeshDetail)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build poly mesh detail");
			return nullptr;
		}

		return polyMeshDetail;
	}


	static NavMeshPtr createNavMesh(const NavMeshData & data) {
		NavMeshPtr navMesh{ dtAllocNavMesh() };
		if (navMesh == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate Detour navmesh");
			return nullptr;
		}

		const auto status = navMesh->init(data.data, data.size, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to init Detour navmesh");
			return nullptr;
		}

		return navMesh;
	}

	static NavMeshQueryPtr createNavMeshQuery(const NavMeshComponent & params, const dtNavMesh & navMesh) {
		NavMeshQueryPtr navMeshQuery{ dtAllocNavMeshQuery() };

		if (navMeshQuery == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate Detour navmesh query");
			return nullptr;
		}

		const auto maxNodes = params.queryMaxSearchNodes;
		kengine_assert(*g_em, 0 < maxNodes && maxNodes <= 65535);
		const auto status = navMeshQuery->init(&navMesh, maxNodes);
		if (dtStatusFailed(status)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to init Detour navmesh query");
			return nullptr;
		}

		return navMeshQuery;
	}
}