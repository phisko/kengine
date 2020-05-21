#include <GL/glew.h>
#include <GL/GL.h>

#include "EntityManager.hpp"

#include "RecastSystem.hpp"
#include "RecastComponent.hpp"
#include "RecastDebugShader.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "functions/Execute.hpp"

#include "helpers/AssertHelper.hpp"

#include "angle.hpp"
#include "with.hpp"

#include "data/ShaderComponent.hpp"

#include "data/ModelComponent.hpp"

namespace Flags {
	enum {
		Walk = 1,
	};
}

namespace kengine {
	static EntityManager * g_em;

	// Adjustables
	static struct {
		float cellSize = .25f;
		float cellHeight = .25f;
		float walkableSlope = putils::pi / 4.f;
		float characterHeight = 1.f;
		float characterClimb = .75f;
		float characterRadius = .5f;
		int maxEdgeLength = 80;
		float maxSimplificationError = 1.1f;
		float minRegionArea = 9.f;
		float mergeRegionArea = 25.f;
		int vertsPerPoly = 6;
		float detailSampleDist = 75.f;
		float detailSampleMaxError = 20.f;
		int queryMaxSearchNodes = 65535;
	} g_adjustables;
	//

	// declarations
	static void execute(float deltaTime);
	//
	EntityCreator * RecastSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::Execute{ execute };

			e += AdjustableComponent{
				"Navmesh", {
					{ "Cell size", &g_adjustables.cellSize },
					{ "Cell height", &g_adjustables.cellHeight },
					{ "Walkable slope", &g_adjustables.walkableSlope },
					{ "Character height", &g_adjustables.characterHeight },
					{ "Character climb", &g_adjustables.characterClimb },
					{ "Character radius", &g_adjustables.characterRadius },
					{ "Max edge length", &g_adjustables.maxEdgeLength },
					{ "Max simplification error", &g_adjustables.maxSimplificationError },
					{ "Min region area", &g_adjustables.minRegionArea },
					{ "Merge region area", &g_adjustables.mergeRegionArea },
					{ "Verts per poly", &g_adjustables.vertsPerPoly },
					{ "Detail sample dist", &g_adjustables.detailSampleDist },
					{ "Detail sample max error", &g_adjustables.detailSampleMaxError },
					{ "Query max search nodes", &g_adjustables.queryMaxSearchNodes }
				}
			};

			e += makeGBufferShaderComponent<RecastDebugShader>(*g_em);
		};
	}

	// declarations
	static void createNavMesh(RecastComponent::Mesh & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	//
	static void execute(float deltaTime) {
		kengine_assert(*g_em, g_adjustables.vertsPerPoly <= DT_VERTS_PER_POLYGON);

		for (auto & [e, modelData, noRecast, model] : g_em->getEntities<ModelDataComponent, no<RecastComponent>, ModelComponent>()) {
			auto & comp = e.attach<RecastComponent>();
			for (const auto & mesh : modelData.meshes) {
				comp.meshes.emplace_back();
				createNavMesh(comp.meshes.back(), modelData, mesh);
			}
		}
	}

	// declarations
	static rcConfig getConfig(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData);
	static CompactHeightfieldPtr createCompactHeightField(rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField);
	static ContourSetPtr createContourSet(rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf);
	static PolyMeshPtr createPolyMesh(rcContext & ctx, const rcConfig & cfg, rcContourSet & contourSet);
	static PolyMeshDetailPtr createPolyMeshDetail(rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcCompactHeightfield & chf);
	static NavMeshPtr createNavMesh(const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcPolyMeshDetail & polyMeshDetail);
	static NavMeshQueryPtr createNavMeshQuery(const dtNavMesh & navMesh);
	//
	static void createNavMesh(RecastComponent::Mesh & navMeshComp, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) {
		const auto cfg = getConfig(modelData, meshData);

		rcContext ctx;
		ctx.resetTimers();
		ctx.startTimer(RC_TIMER_TOTAL);

		navMeshComp.heightField = createHeightField(ctx, cfg, meshData);
		if (navMeshComp.heightField == nullptr)
			return;

		rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *navMeshComp.heightField);
		rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *navMeshComp.heightField);
		rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *navMeshComp.heightField);

		navMeshComp.compactHeightField = createCompactHeightField(ctx, cfg, *navMeshComp.heightField);
		if (navMeshComp.compactHeightField == nullptr)
			return;

		navMeshComp.contourSet = createContourSet(ctx, cfg, *navMeshComp.compactHeightField);
		if (navMeshComp.contourSet == nullptr)
			return;

		navMeshComp.polyMesh = createPolyMesh(ctx, cfg, *navMeshComp.contourSet);
		if (navMeshComp.polyMesh == nullptr)
			return;

		navMeshComp.polyMeshDetail = createPolyMeshDetail(ctx, cfg, *navMeshComp.polyMesh, *navMeshComp.compactHeightField);
		if (navMeshComp.polyMeshDetail == nullptr)
			return;

		// Build for detour
		bool found = false;
		for (int i = 0; i < navMeshComp.polyMesh->npolys; ++i)
			if (navMeshComp.polyMesh->areas[i] == RC_WALKABLE_AREA) {
				navMeshComp.polyMesh->flags[i] = Flags::Walk;
				found = true;
			}
		if (!found)
			return;

		navMeshComp.navMesh = createNavMesh(cfg, *navMeshComp.polyMesh, *navMeshComp.polyMeshDetail);
		if (navMeshComp.navMesh == nullptr)
			return;

		navMeshComp.navMeshQuery = createNavMeshQuery(*navMeshComp.navMesh);
		if (navMeshComp.navMeshQuery == nullptr)
			return;

		ctx.stopTimer(RC_TIMER_TOTAL);
	}

	// declarations
	std::unique_ptr<float[]> getVertices(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	//
	static rcConfig getConfig(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) {
		rcConfig cfg;
		memset(&cfg, 0, sizeof(cfg));

		{ putils_with(g_adjustables) {
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

		const auto vertices = getVertices(modelData, meshData);
		if (vertices == nullptr)
			return cfg;

		rcCalcBounds(vertices.get(), (int)meshData.vertices.nbElements, cfg.bmin, cfg.bmax);
		rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

		return cfg;
	}

	// declarations
	const float * getVertexPosition(const ModelDataComponent & modelData, const void * vertices, size_t vertex);
	//
	std::unique_ptr<float[]> getVertices(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) {
		auto vertices = std::unique_ptr<float[]>(new float[meshData.vertices.nbElements * 3]);

		for (size_t vertex = 0; vertex < meshData.vertices.nbElements; ++vertex) {
			const auto pos = getVertexPosition(modelData, meshData.vertices.data, vertex);
			if (pos == nullptr)
				return nullptr;
			for (size_t i = 0; i < 3; ++i)
				vertices[vertex * 3 + i] = pos[i];
		}

		return vertices;
	}

	const float * getVertexPosition(const ModelDataComponent & modelData, const void * vertices, size_t vertex) {
		static const char * potentialNames[] = { "pos", "position" };

		const void * pos = nullptr;
		for (const auto name : potentialNames) {
			pos = modelData.getVertexAttribute(vertices, vertex, name);
			if (pos != nullptr)
				break;
		}

		if (pos == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Could not find vertex position");
			return nullptr;
		}

		return (const float *)pos;
	}

	static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData) {
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
			{
				indices[i] = (int)unsignedIndices[i];
				assert(indices[i] >= 0 && indices[i] < meshData.vertices.nbElements);
			}
		}

		rcMarkWalkableTriangles(&ctx, cfg.walkableSlopeAngle,
			(const float *)meshData.vertices.data, (int)meshData.vertices.nbElements,
			indices, (int)nbTriangles,
			triangleAreas);

		if (mustDeleteIndices)
			delete[] indices;

		if (!rcRasterizeTriangles(&ctx, (const float *)meshData.vertices.data, triangleAreas, (int)nbTriangles, *heightField, cfg.walkableClimb)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to rasterize triangles");
			delete[] triangleAreas;
			return nullptr;
		}

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

	static NavMeshPtr createNavMesh(const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcPolyMeshDetail & polyMeshDetail) {
		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		{ putils_with(polyMesh) {
			params.verts = _.verts;
			params.vertCount = _.nverts;
			params.polys = _.polys;
			params.polyAreas = _.areas;
			params.polyFlags = _.flags;
			params.polyCount = _.npolys;
			params.nvp = _.nvp;
		} }

		{ putils_with(polyMeshDetail) {
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

		unsigned char * navMeshData = nullptr;
		int navMeshDataSize;
		if (!dtCreateNavMeshData(&params, &navMeshData, &navMeshDataSize)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to create Detour navmesh data");
			return nullptr;
		}

		NavMeshPtr navMesh{ dtAllocNavMesh() };
		if (navMesh == nullptr) {
			dtFree(navMeshData);
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate Detour navmesh");
			return nullptr;
		}

		const auto status = navMesh->init(navMeshData, navMeshDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to init Detour navmesh");
			return nullptr;
		}

		return navMesh;
	}

	static NavMeshQueryPtr createNavMeshQuery(const dtNavMesh & navMesh) {
		NavMeshQueryPtr navMeshQuery{ dtAllocNavMeshQuery() };

		if (navMeshQuery == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate Detour navmesh query");
			return nullptr;
		}

		const auto maxNodes = g_adjustables.queryMaxSearchNodes;
		kengine_assert(*g_em, 0 < maxNodes && maxNodes <= 65535);
		const auto status = navMeshQuery->init(&navMesh, maxNodes);
		if (dtStatusFailed(status)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to init Detour navmesh query");
			return nullptr;
		}

		return navMeshQuery;
	}
}