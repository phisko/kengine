#include <GL/glew.h>
#include <GL/GL.h>

#include "RecastSystem.hpp"
#include "EntityManager.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "functions/Execute.hpp"

#include "helpers/AssertHelper.hpp"

#include "angle.hpp"
#include "with.hpp"

#include <Recast.h>
#include <DetourNavMesh.h>

#pragma region RAII
template<typename T, void (*FreeFunc)(T *)>
struct Deleter {
	void operator()(T * ptr) { FreeFunc(ptr); }
};

template<typename T, void(*FreeFunc)(T *)>
using UniquePtr = std::unique_ptr<T, Deleter<T, FreeFunc>>;

using HeightfieldPtr = UniquePtr<rcHeightfield, rcFreeHeightField>;
using CompactHeightfieldPtr = UniquePtr<rcCompactHeightfield, rcFreeCompactHeightfield>;
using ContourSetPtr = UniquePtr<rcContourSet, rcFreeContourSet>;
using PolyMeshPtr = UniquePtr<rcPolyMesh, rcFreePolyMesh>;
using PolyMeshDetailPtr = UniquePtr<rcPolyMeshDetail, rcFreePolyMeshDetail>;
#pragma endregion

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
					{ "Detail sample max error", &g_adjustables.detailSampleMaxError }
				}
			};
		};
	}

	struct RecastComponent {
		struct Mesh {
			PolyMeshPtr polyMesh;
			PolyMeshDetailPtr polyMeshDetail;
		};

		std::vector<Mesh> meshes;
	};

	// declarations
	static void createNavMesh(RecastComponent::Mesh & navMesh, const ModelDataComponent::Mesh & meshData);
	//
	static void execute(float deltaTime) {
		kengine_assert(*g_em, g_adjustables.vertsPerPoly <= DT_VERTS_PER_POLYGON);

		for (auto & [e, modelData, noRecast] : g_em->getEntities<ModelDataComponent, no<RecastComponent>>()) {
			auto & comp = e.attach<RecastComponent>();
			for (const auto & mesh : modelData.meshes) {
				comp.meshes.emplace_back();
				createNavMesh(comp.meshes.back(), mesh);
			}
		}
	}

	// declarations
	static rcConfig getConfig(const ModelDataComponent::Mesh & meshData);
	static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData);
	static CompactHeightfieldPtr createCompactHeightField(rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField);
	//
	static void createNavMesh(RecastComponent::Mesh & navMesh, const ModelDataComponent::Mesh & meshData) {
		const auto cfg = getConfig(meshData);

		rcContext ctx;
		ctx.resetTimers();
		ctx.startTimer(RC_TIMER_TOTAL);

		const auto heightField = createHeightField(ctx, cfg, meshData);
		if (heightField == nullptr)
			return;

		rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *heightField);
		rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField);
		rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *heightField);

		const auto compactHeightField = createCompactHeightField(ctx, cfg, *heightField);
		if (compactHeightField == nullptr)
			return;

		if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *compactHeightField)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to erode walkable area");
			return;
		}
		
		// Classic recast positiong. For others, see https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_SoloMesh.cpp
		if (!rcBuildDistanceField(&ctx, *compactHeightField)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build distance field");
			return;
		}

		if (!rcBuildRegions(&ctx, *compactHeightField, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build regions");
			return;
		}

		const ContourSetPtr contourSet{ rcAllocContourSet() };
		if (contourSet == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate contour set");
			return;
		}

		if (!rcBuildContours(&ctx, *compactHeightField, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build contours");
			return;
		}

		navMesh.polyMesh.reset(rcAllocPolyMesh());
		if (navMesh.polyMesh == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate poly mesh");
			return;
		}
		
		if (!rcBuildPolyMesh(&ctx, *contourSet, cfg.maxVertsPerPoly, *navMesh.polyMesh)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build poly mesh");
			return;
		}

		navMesh.polyMeshDetail.reset(rcAllocPolyMeshDetail());
		if (navMesh.polyMeshDetail == nullptr) {
			kengine_assert_failed(*g_em, "[Recast] Failed to allocate poly mesh detail");
			return;
		}

		if (!rcBuildPolyMeshDetail(&ctx, *navMesh.polyMesh, *compactHeightField, cfg.detailSampleDist, cfg.detailSampleMaxError, *navMesh.polyMeshDetail)) {
			kengine_assert_failed(*g_em, "[Recast] Failed to build poly mesh detail");
			return;
		}
	}

	static rcConfig getConfig(const ModelDataComponent::Mesh & meshData) {
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

		rcCalcBounds((const float *)meshData.vertices.data, (int)meshData.vertices.nbElements, cfg.bmin, cfg.bmax);
		rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

		return cfg;
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

		return compactHeightField;
	}
}