// stl
#include <filesystem>
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// recast
#include <Recast.h>
#include <DetourNavMeshBuilder.h>

// putils
#include "lengthof.hpp"
#include "on_scope_exit.hpp"
#include "with.hpp"

// kengine data
#include "data/ModelComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/NavMeshComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "Common.hpp"
#include "RecastNavMeshComponent.hpp"

namespace kengine::recast {
	struct buildRecastComponent {
		static void run(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto model = r.try_get<ModelComponent>(e);
			const auto modelData = r.try_get<ModelDataComponent>(e);
			const auto navMesh = r.try_get<NavMeshComponent>(e);

			if (model && modelData && navMesh) {
				kengine_logf(r, Verbose, "Execute/RecastSystem", "Building navmesh for %zu", e);
				kengine_assert(r, navMesh->vertsPerPoly <= DT_VERTS_PER_POLYGON);
				createRecastMesh(model->file.c_str(), { r, e }, *navMesh, *modelData);
			}
		}

		using HeightfieldPtr = UniquePtr<rcHeightfield, rcFreeHeightField>;
		using CompactHeightfieldPtr = UniquePtr<rcCompactHeightfield, rcFreeCompactHeightfield>;
		using ContourSetPtr = UniquePtr<rcContourSet, rcFreeContourSet>;
		using PolyMeshPtr = UniquePtr<rcPolyMesh, rcFreePolyMesh>;
		using PolyMeshDetailPtr = UniquePtr<rcPolyMeshDetail, rcFreePolyMeshDetail>;

		struct NavMeshData {
			unsigned char * data = nullptr;
			int size = 0;
			float areaSize = 0.f;
		};

		static void createRecastMesh(const char * file, entt::handle e, NavMeshComponent & navMesh, const ModelDataComponent & modelData) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & r = *e.registry();

			NavMeshData data;

			const putils::string<4096> binaryFile("%s.nav", file);
			bool mustSave = false;
			data = loadBinaryFile(binaryFile.c_str(), navMesh);
			if (data.data == nullptr) {
				kengine_logf(r, Verbose, "Execute/RecastSystem/createRecastMesh", "Found no binary file for %s, creating nav mesh data", file);
				data = createNavMeshData(r, navMesh, modelData, modelData.meshes[navMesh.concernedMesh]);
				if (data.data == nullptr)
					return;
				mustSave = true;
			}
			else
				kengine_logf(r, Verbose, "Execute/RecastSystem/createRecastMesh", "Found binary file for %s", file);

			auto & recast = e.get_or_emplace<RecastNavMeshComponent>();
			recast.navMesh = createNavMesh(r, data);
			if (recast.navMesh == nullptr) {
				dtFree(data.data);
				return;
			}

			recast.navMeshQuery = createNavMeshQuery(r, navMesh, *recast.navMesh);
			if (recast.navMeshQuery == nullptr) {
				dtFree(data.data);
				return;
			}

			if (mustSave)
				saveBinaryFile(r, binaryFile.c_str(), data, navMesh);

			(void)e.get_or_emplace<functions::GetPath>(getPath(e.try_get<TransformComponent>(), navMesh, recast));
		}

		static NavMeshData loadBinaryFile(const char * binaryFile, const NavMeshComponent & navMesh) noexcept {
			KENGINE_PROFILING_SCOPE;

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

		static void saveBinaryFile(const entt::registry & r, const char * binaryFile, const NavMeshData & data, const NavMeshComponent & navMesh) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, Verbose, "Execute/RecastSystem/createRecastMesh", "Saving binary file %s", binaryFile);

			std::ofstream f(binaryFile, std::ofstream::trunc | std::ofstream::binary);
			f.write((const char *)&navMesh, sizeof(navMesh));
			f.write((const char *)&data.size, sizeof(data.size));
			f.write((const char *)data.data, data.size);
		}

		static NavMeshData createNavMeshData(const entt::registry & r, const NavMeshComponent & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) noexcept {
			KENGINE_PROFILING_SCOPE;

			NavMeshData ret;

			const auto vertices = getVertices(r, modelData, meshData);

			const auto cfg = getConfig(r, navMesh, meshData, vertices.get());
			if (cfg.width == 0 || cfg.height == 0) {
				kengine_assert_failed(r, "[Recast] Mesh was 0 height or width?");
				return ret;
			}

			rcContext ctx;

			const auto heightField = createHeightField(r, ctx, cfg, meshData, vertices.get());
			if (heightField == nullptr)
				return ret;

			rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *heightField);
			rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *heightField);
			rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *heightField);

			const auto compactHeightField = createCompactHeightField(r, ctx, cfg, *heightField);
			if (compactHeightField == nullptr)
				return ret;

			const auto contourSet = createContourSet(r, ctx, cfg, *compactHeightField);
			if (contourSet == nullptr)
				return ret;

			const auto polyMesh = createPolyMesh(r, ctx, cfg, *contourSet);
			if (polyMesh == nullptr)
				return ret;

			const auto polyMeshDetail = createPolyMeshDetail(r, ctx, cfg, *polyMesh, *compactHeightField);
			if (polyMeshDetail == nullptr)
				return ret;

			for (int i = 0; i < polyMesh->npolys; ++i)
				if (polyMesh->areas[i] == RC_WALKABLE_AREA)
					polyMesh->flags[i] = Flags::Walk;

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
				kengine_assert_failed(r, "[Recast] Failed to create Detour navmesh data");

			ret.areaSize = putils::getLength(putils::Point3f(cfg.bmax) - putils::Point3f(cfg.bmin));
			return ret;
		}

		static std::unique_ptr<float[]> getVertices(const entt::registry & r, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto positionOffset = getVertexPositionOffset(r, modelData);
			if (positionOffset == std::nullopt)
				return nullptr;

			auto vertices = std::unique_ptr<float[]>(new float[meshData.vertices.nbElements * 3]);

			for (size_t vertex = 0; vertex < meshData.vertices.nbElements; ++vertex) {
				const auto pos = getVertexPosition(meshData.vertices.data, vertex, modelData.vertexSize, *positionOffset);
				for (size_t i = 0; i < 3; ++i)
					vertices[vertex * 3 + i] = pos[i];
			}

			return vertices;
		}

		static std::optional<std::ptrdiff_t> getVertexPositionOffset(const entt::registry & r, const ModelDataComponent & modelData) noexcept {
			KENGINE_PROFILING_SCOPE;

			static const std::string_view potentialNames[] = { "pos", "position" };

			for (const auto name : potentialNames)
				for (const auto & attribute : modelData.vertexAttributes)
					if (attribute.name == name)
						return attribute.offset;

			kengine_assert_failed(r, "[Recast] Could not find vertex position");
			return std::nullopt;
		}

		static const float * getVertexPosition(const void * vertices, size_t index, size_t vertexSize, std::ptrdiff_t positionOffset) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto vertex = (const char *)vertices + index * vertexSize;
			return (const float *)(vertex + positionOffset);
		}

		static rcConfig getConfig(const entt::registry & r, const NavMeshComponent & navMesh, const ModelDataComponent::Mesh & meshData, const float * vertices) noexcept {
			KENGINE_PROFILING_SCOPE;

			rcConfig cfg;
			memset(&cfg, 0, sizeof(cfg));

			{ putils_with(navMesh) {
				cfg.cs = _.cellSize;
				kengine_assert(r, cfg.cs > 0);

				cfg.ch = _.cellHeight;
				kengine_assert(r, cfg.ch > 0);

				cfg.walkableSlopeAngle = putils::toDegrees(_.walkableSlope);
				kengine_assert(r, cfg.walkableSlopeAngle > 0.f && cfg.walkableSlopeAngle <= 90.f);

				cfg.walkableHeight = (int)ceilf(_.characterHeight / _.cellHeight);
				kengine_assert(r, cfg.walkableHeight >= 3);

				cfg.walkableClimb = (int)floorf(_.characterClimb / _.cellHeight);
				kengine_assert(r, cfg.walkableClimb >= 0);

				cfg.walkableRadius = (int)ceilf(_.characterRadius / _.cellSize);
				kengine_assert(r, cfg.walkableRadius >= 0);

				cfg.maxEdgeLen = (int)(_.maxEdgeLength / _.cellSize);
				kengine_assert(r, cfg.maxEdgeLen >= 0);

				cfg.maxSimplificationError = _.maxSimplificationError;
				kengine_assert(r, cfg.maxSimplificationError >= 0);

				cfg.minRegionArea = (int)rcSqr(_.minRegionArea);
				kengine_assert(r, cfg.minRegionArea >= 0);

				cfg.mergeRegionArea = (int)rcSqr(_.mergeRegionArea);
				kengine_assert(r, cfg.mergeRegionArea >= 0);

				cfg.maxVertsPerPoly = _.vertsPerPoly;
				kengine_assert(r, cfg.maxVertsPerPoly >= 3);

				cfg.detailSampleDist = _.detailSampleDist;
				kengine_assert(r, cfg.detailSampleDist == 0.f || cfg.detailSampleDist >= .9f);

				cfg.detailSampleMaxError = _.detailSampleMaxError;
				kengine_assert(r, cfg.detailSampleMaxError >= 0.f);
			} }

			rcCalcBounds(vertices, (int)meshData.vertices.nbElements, cfg.bmin, cfg.bmax);
			rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

			return cfg;
		}

		static HeightfieldPtr createHeightField(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData, const float * vertices) noexcept {
			KENGINE_PROFILING_SCOPE;

			HeightfieldPtr heightField{ rcAllocHeightfield() };

			if (heightField == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate height field");
				return nullptr;
			}

			if (!rcCreateHeightfield(&ctx, *heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) {
				kengine_assert_failed(r, "[Recast] Failed to create height field");
				return nullptr;
			}

			const auto nbTriangles = meshData.indices.nbElements / 3; // I think?
			const auto triangleAreas = new unsigned char[nbTriangles];
			memset(triangleAreas, 0, nbTriangles);

			int * indices = (int *)meshData.indices.data;
			bool mustDeleteIndices = false;
			if (meshData.indexType == putils::meta::type<unsigned int>::index) {
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
				kengine_assert_failed(r, "[Recast] Failed to rasterize triangles");
				delete[] triangleAreas;
				return nullptr;
			}

			if (mustDeleteIndices)
				delete[] indices;

			delete[] triangleAreas;

			return heightField;
		}

		static CompactHeightfieldPtr createCompactHeightField(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField) noexcept {
			KENGINE_PROFILING_SCOPE;

			CompactHeightfieldPtr compactHeightField{ rcAllocCompactHeightfield() };

			if (compactHeightField == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate compact height field");
				return nullptr;
			}

			if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, heightField, *compactHeightField)) {
				kengine_assert_failed(r, "[Recast] Failed to build compact height field");
				return nullptr;
			}

			if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *compactHeightField)) {
				kengine_assert_failed(r, "[Recast] Failed to erode walkable area");
				return nullptr;
			}

			// Classic recast positiong. For others, see https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_SoloMesh.cpp
			if (!rcBuildDistanceField(&ctx, *compactHeightField)) {
				kengine_assert_failed(r, "[Recast] Failed to build distance field");
				return nullptr;
			}

			if (!rcBuildRegions(&ctx, *compactHeightField, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
				kengine_assert_failed(r, "[Recast] Failed to build regions");
				return nullptr;
			}

			return compactHeightField;
		}

		static ContourSetPtr createContourSet(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf) noexcept {
			KENGINE_PROFILING_SCOPE;

			ContourSetPtr contourSet{ rcAllocContourSet() };

			if (contourSet == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate contour set");
				return nullptr;
			}

			if (!rcBuildContours(&ctx, chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet)) {
				kengine_assert_failed(r, "[Recast] Failed to build contours");
				return nullptr;
			}

			return contourSet;
		}

		static PolyMeshPtr createPolyMesh(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcContourSet & contourSet) noexcept {
			KENGINE_PROFILING_SCOPE;

			PolyMeshPtr polyMesh{ rcAllocPolyMesh() };

			if (polyMesh == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate poly mesh");
				return nullptr;
			}

			if (!rcBuildPolyMesh(&ctx, contourSet, cfg.maxVertsPerPoly, *polyMesh)) {
				kengine_assert_failed(r, "[Recast] Failed to build poly mesh");
				return nullptr;
			}

			return polyMesh;
		}

		static PolyMeshDetailPtr createPolyMeshDetail(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcCompactHeightfield & chf) noexcept {
			KENGINE_PROFILING_SCOPE;

			PolyMeshDetailPtr polyMeshDetail{ rcAllocPolyMeshDetail() };
			if (polyMeshDetail == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate poly mesh detail");
				return nullptr;
			}

			if (!rcBuildPolyMeshDetail(&ctx, polyMesh, chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *polyMeshDetail)) {
				kengine_assert_failed(r, "[Recast] Failed to build poly mesh detail");
				return nullptr;
			}

			return polyMeshDetail;
		}

		static NavMeshPtr createNavMesh(const entt::registry & r, const NavMeshData & data) noexcept {
			KENGINE_PROFILING_SCOPE;

			NavMeshPtr navMesh{ dtAllocNavMesh() };
			if (navMesh == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate Detour navmesh");
				return nullptr;
			}

			const auto status = navMesh->init(data.data, data.size, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status)) {
				kengine_assert_failed(r, "[Recast] Failed to init Detour navmesh");
				return nullptr;
			}

			return navMesh;
		}

		static NavMeshQueryPtr createNavMeshQuery(const entt::registry & r, const NavMeshComponent & params, const dtNavMesh & navMesh) noexcept {
			KENGINE_PROFILING_SCOPE;

			NavMeshQueryPtr navMeshQuery{ dtAllocNavMeshQuery() };

			if (navMeshQuery == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate Detour navmesh query");
				return nullptr;
			}

			const auto maxNodes = params.queryMaxSearchNodes;
			kengine_assert(r, 0 < maxNodes && maxNodes <= 65535);
			const auto status = navMeshQuery->init(&navMesh, maxNodes);
			if (dtStatusFailed(status)) {
				kengine_assert_failed(r, "[Recast] Failed to init Detour navmesh query");
				return nullptr;
			}

			return navMeshQuery;
		}

		static functions::GetPath::Callable getPath(const TransformComponent * modelTransform, const NavMeshComponent & navMesh, const RecastNavMeshComponent & recast) noexcept {
			KENGINE_PROFILING_SCOPE;

			return [&, modelTransform](entt::handle environment, const putils::Point3f & startWorldSpace, const putils::Point3f & endWorldSpace) {
				KENGINE_PROFILING_SCOPE;
				kengine_logf(*environment.registry(), Verbose, "RecastSystem", "Getting path in %zu from { %f, %f, %f } to { %f, %f, %f }",
					environment, startWorldSpace.x, startWorldSpace.y, startWorldSpace.z, endWorldSpace.x, endWorldSpace.y, endWorldSpace.z);
				static const dtQueryFilter filter;

				const auto modelToWorld = matrixHelper::getModelMatrix(environment.get<TransformComponent>(), modelTransform);
				const auto worldToModel = glm::inverse(modelToWorld);

				const auto start = matrixHelper::convertToReferencial(startWorldSpace, worldToModel);
				const auto end = matrixHelper::convertToReferencial(endWorldSpace, worldToModel);

				functions::GetPathImpl::Path ret;

				const auto maxExtent = std::max(navMesh.characterRadius * 2.f, navMesh.characterHeight);
				const float extents[3] = { maxExtent, maxExtent, maxExtent };

				dtPolyRef startRef;
				float startPt[3];
				auto status = recast.navMeshQuery->findNearestPoly(start.raw, extents, &filter, &startRef, startPt);
				if (dtStatusFailed(status) || startRef == 0) {
					kengine_log(*environment.registry(), Error, "RecastSystem/getPath", "Failed to find nearest poly to start");
					return ret;
				}

				dtPolyRef endRef;
				float endPt[3];
				status = recast.navMeshQuery->findNearestPoly(end.raw, extents, &filter, &endRef, endPt);
				if (dtStatusFailed(status) || endRef == 0) {
					kengine_log(*environment.registry(), Error, "RecastSystem/getPath", "Failed to find nearest poly to end");
					return ret;
				}

				dtPolyRef path[KENGINE_NAVMESH_MAX_PATH_LENGTH];
				int pathCount = 0;
				status = recast.navMeshQuery->findPath(startRef, endRef, startPt, endPt, &filter, path, &pathCount, (int)putils::lengthof(path));
				if (dtStatusFailed(status)) {
					kengine_log(*environment.registry(), Error, "RecastSystem/getPath", "Failed to find path");
					return ret;
				}

				ret.resize(ret.capacity());
				int straightPathCount = 0;

				static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
				status = recast.navMeshQuery->findStraightPath(startPt, endPt, path, pathCount, ret[0].raw, nullptr, nullptr, &straightPathCount, (int)ret.capacity());
				if (dtStatusFailed(status)) {
					kengine_log(*environment.registry(), Error, "RecastSystem/getPath", "Failed to find straight path");
					return ret;
				}

				ret.resize(straightPathCount);
				for (auto & step : ret)
					step = matrixHelper::convertToReferencial(step, modelToWorld);

				return ret;
			};
		}

		enum Flags {
			Walk = 1,
		};
	};

	void buildRecastComponent(entt::registry & r, entt::entity e) noexcept {
		buildRecastComponent::run(r, e);
	}
}
