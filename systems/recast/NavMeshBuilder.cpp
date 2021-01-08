#include <filesystem>
#include <fstream>

#include <Recast.h>
#include <DetourNavMeshBuilder.h>

#include "kengine.hpp"

#include "data/ModelComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/NavMeshComponent.hpp"
#include "data/TransformComponent.hpp"

#include "helpers/assertHelper.hpp"
#include "helpers/matrixHelper.hpp"

#include "Common.hpp"
#include "RecastNavMeshComponent.hpp"

#include "lengthof.hpp"
#include "with.hpp"

namespace Flags {
	enum {
		Walk = 1,
	};
}

namespace kengine::recast {
	void buildNavMeshes() noexcept {
		struct impl {
			static void buildNavMeshes() noexcept {
				std::atomic<size_t> jobsLeft = 0;

				static const auto buildRecastComponent = [&](auto && entities) noexcept {
					for (auto [e, model, modelData, navMesh, _] : entities) {
						++jobsLeft;
						threadPool().runTask([&, id = e.id]() noexcept {
							kengine_assert(navMesh.vertsPerPoly <= DT_VERTS_PER_POLYGON);
							createRecastMesh(model.file, kengine::entities.get(id), navMesh, modelData);
							if constexpr (std::is_same<RebuildNavMeshComponent, putils_typeof(_)>())
								e.detach<RebuildNavMeshComponent>();
							--jobsLeft;
						});
					}
				};

				buildRecastComponent(kengine::entities.with<ModelComponent, ModelDataComponent, NavMeshComponent, no<RecastNavMeshComponent>>());
				buildRecastComponent(kengine::entities.with<ModelComponent, ModelDataComponent, NavMeshComponent, RebuildNavMeshComponent>());
				while (jobsLeft > 0);
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

			static void createRecastMesh(const char * file, Entity e, NavMeshComponent & navMesh, const ModelDataComponent & modelData) noexcept {
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

				auto & recast = e.attach<RecastNavMeshComponent>();
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

				navMesh.getPath = getPath(e.get<ModelComponent>(), navMesh, recast);
			}

			static NavMeshData loadBinaryFile(const char * binaryFile, const NavMeshComponent & navMesh) noexcept {
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

			static void saveBinaryFile(const char * binaryFile, const NavMeshData & data, const NavMeshComponent & navMesh) noexcept {
				std::ofstream f(binaryFile, std::ofstream::trunc | std::ofstream::binary);
				f.write((const char *)&navMesh, sizeof(navMesh));
				f.write((const char *)&data.size, sizeof(data.size));
				f.write((const char *)data.data, data.size);
			}

			static NavMeshData createNavMeshData(const NavMeshComponent & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) noexcept {
				NavMeshData ret;

				const auto vertices = getVertices(modelData, meshData);

				const auto cfg = getConfig(navMesh, meshData, vertices.get());
				if (cfg.width == 0 || cfg.height == 0) {
					kengine_assert_failed("[Recast] Mesh was 0 height or width?");
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
					kengine_assert_failed("[Recast] Failed to create Detour navmesh data");

				ret.areaSize = putils::getLength(putils::Point3f(cfg.bmax) - putils::Point3f(cfg.bmin));
				return ret;
			}

			static std::unique_ptr<float[]> getVertices(const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData) noexcept {
				const auto positionOffset = getVertexPositionOffset(modelData);
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

			static std::optional<std::ptrdiff_t> getVertexPositionOffset(const ModelDataComponent & modelData) noexcept {
				static const char * potentialNames[] = { "pos", "position" };

				for (const auto name : potentialNames)
					for (const auto & attribute : modelData.vertexAttributes)
						if (strcmp(attribute.name, name) == 0)
							return attribute.offset;

				kengine_assert_failed("[Recast] Could not find vertex position");
				return std::nullopt;
			}

			static const float * getVertexPosition(const void * vertices, size_t index, size_t vertexSize, std::ptrdiff_t positionOffset) noexcept {
				const auto vertex = (const char *)vertices + index * vertexSize;
				return (const float *)(vertex + positionOffset);
			}

			static rcConfig getConfig(const NavMeshComponent & navMesh, const ModelDataComponent::Mesh & meshData, const float * vertices) noexcept {
				rcConfig cfg;
				memset(&cfg, 0, sizeof(cfg));

				{ putils_with(navMesh) {
					cfg.cs = _.cellSize;
					kengine_assert(cfg.cs > 0);

					cfg.ch = _.cellHeight;
					kengine_assert(cfg.ch > 0);

					cfg.walkableSlopeAngle = putils::toDegrees(_.walkableSlope);
					kengine_assert(cfg.walkableSlopeAngle > 0.f && cfg.walkableSlopeAngle <= 90.f);

					cfg.walkableHeight = (int)ceilf(_.characterHeight / _.cellHeight);
					kengine_assert(cfg.walkableHeight >= 3);

					cfg.walkableClimb = (int)floorf(_.characterClimb / _.cellHeight);
					kengine_assert(cfg.walkableClimb >= 0);

					cfg.walkableRadius = (int)ceilf(_.characterRadius / _.cellSize);
					kengine_assert(cfg.walkableRadius >= 0);

					cfg.maxEdgeLen = (int)(_.maxEdgeLength / _.cellSize);
					kengine_assert(cfg.maxEdgeLen >= 0);

					cfg.maxSimplificationError = _.maxSimplificationError;
					kengine_assert(cfg.maxSimplificationError >= 0);

					cfg.minRegionArea = (int)rcSqr(_.minRegionArea);
					kengine_assert(cfg.minRegionArea >= 0);

					cfg.mergeRegionArea = (int)rcSqr(_.mergeRegionArea);
					kengine_assert(cfg.mergeRegionArea >= 0);

					cfg.maxVertsPerPoly = _.vertsPerPoly;
					kengine_assert(cfg.maxVertsPerPoly >= 3);

					cfg.detailSampleDist = _.detailSampleDist;
					kengine_assert(cfg.detailSampleDist == 0.f || cfg.detailSampleDist >= .9f);

					cfg.detailSampleMaxError = _.detailSampleMaxError;
					kengine_assert(cfg.detailSampleMaxError >= 0.f);
				} }

				rcCalcBounds(vertices, (int)meshData.vertices.nbElements, cfg.bmin, cfg.bmax);
				rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

				return cfg;
			}

			static HeightfieldPtr createHeightField(rcContext & ctx, const rcConfig & cfg, const kengine::ModelDataComponent::Mesh & meshData, const float * vertices) noexcept {
				HeightfieldPtr heightField{ rcAllocHeightfield() };

				if (heightField == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate height field");
					return nullptr;
				}

				if (!rcCreateHeightfield(&ctx, *heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) {
					kengine_assert_failed("[Recast] Failed to create height field");
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
					kengine_assert_failed("[Recast] Failed to rasterize triangles");
					delete[] triangleAreas;
					return nullptr;
				}

				if (mustDeleteIndices)
					delete[] indices;

				delete[] triangleAreas;

				return heightField;
			}

			static CompactHeightfieldPtr createCompactHeightField(rcContext & ctx, const rcConfig & cfg, rcHeightfield & heightField) noexcept {
				CompactHeightfieldPtr compactHeightField{ rcAllocCompactHeightfield() };

				if (compactHeightField == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate compact height field");
					return nullptr;
				}

				if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, heightField, *compactHeightField)) {
					kengine_assert_failed("[Recast] Failed to build compact height field");
					return nullptr;
				}

				if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *compactHeightField)) {
					kengine_assert_failed("[Recast] Failed to erode walkable area");
					return nullptr;
				}

				// Classic recast positiong. For others, see https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_SoloMesh.cpp
				if (!rcBuildDistanceField(&ctx, *compactHeightField)) {
					kengine_assert_failed("[Recast] Failed to build distance field");
					return nullptr;
				}

				if (!rcBuildRegions(&ctx, *compactHeightField, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
					kengine_assert_failed("[Recast] Failed to build regions");
					return nullptr;
				}

				return compactHeightField;
			}

			static ContourSetPtr createContourSet(rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf) noexcept {
				ContourSetPtr contourSet{ rcAllocContourSet() };

				if (contourSet == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate contour set");
					return nullptr;
				}

				if (!rcBuildContours(&ctx, chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet)) {
					kengine_assert_failed("[Recast] Failed to build contours");
					return nullptr;
				}

				return contourSet;
			}

			static PolyMeshPtr createPolyMesh(rcContext & ctx, const rcConfig & cfg, rcContourSet & contourSet) noexcept {
				PolyMeshPtr polyMesh{ rcAllocPolyMesh() };

				if (polyMesh == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate poly mesh");
					return nullptr;
				}

				if (!rcBuildPolyMesh(&ctx, contourSet, cfg.maxVertsPerPoly, *polyMesh)) {
					kengine_assert_failed("[Recast] Failed to build poly mesh");
					return nullptr;
				}

				return polyMesh;
			}

			static PolyMeshDetailPtr createPolyMeshDetail(rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & polyMesh, const rcCompactHeightfield & chf) noexcept {
				PolyMeshDetailPtr polyMeshDetail{ rcAllocPolyMeshDetail() };
				if (polyMeshDetail == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate poly mesh detail");
					return nullptr;
				}

				if (!rcBuildPolyMeshDetail(&ctx, polyMesh, chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *polyMeshDetail)) {
					kengine_assert_failed("[Recast] Failed to build poly mesh detail");
					return nullptr;
				}

				return polyMeshDetail;
			}

			static NavMeshPtr createNavMesh(const NavMeshData & data) noexcept {
				NavMeshPtr navMesh{ dtAllocNavMesh() };
				if (navMesh == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate Detour navmesh");
					return nullptr;
				}

				const auto status = navMesh->init(data.data, data.size, DT_TILE_FREE_DATA);
				if (dtStatusFailed(status)) {
					kengine_assert_failed("[Recast] Failed to init Detour navmesh");
					return nullptr;
				}

				return navMesh;
			}

			static NavMeshQueryPtr createNavMeshQuery(const NavMeshComponent & params, const dtNavMesh & navMesh) noexcept {
				NavMeshQueryPtr navMeshQuery{ dtAllocNavMeshQuery() };

				if (navMeshQuery == nullptr) {
					kengine_assert_failed("[Recast] Failed to allocate Detour navmesh query");
					return nullptr;
				}

				const auto maxNodes = params.queryMaxSearchNodes;
				kengine_assert(0 < maxNodes && maxNodes <= 65535);
				const auto status = navMeshQuery->init(&navMesh, maxNodes);
				if (dtStatusFailed(status)) {
					kengine_assert_failed("[Recast] Failed to init Detour navmesh query");
					return nullptr;
				}

				return navMeshQuery;
			}

			static NavMeshComponent::GetPathFunc getPath(const ModelComponent & model, const NavMeshComponent & navMesh, const RecastNavMeshComponent & recast) noexcept {
				return [&](const Entity & environment, const putils::Point3f & startWorldSpace, const putils::Point3f & endWorldSpace) {
					static const dtQueryFilter filter;

					const auto modelToWorld = matrixHelper::getModelMatrix(model, environment.get<TransformComponent>());
					const auto worldToModel = glm::inverse(modelToWorld);

					const auto start = matrixHelper::convertToReferencial(startWorldSpace, worldToModel);
					const auto end = matrixHelper::convertToReferencial(endWorldSpace, worldToModel);

					NavMeshComponent::Path ret;

					const auto maxExtent = std::max(navMesh.characterRadius * 2.f, navMesh.characterHeight);
					const float extents[3] = { maxExtent, maxExtent, maxExtent };

					dtPolyRef startRef;
					float startPt[3];
					auto status = recast.navMeshQuery->findNearestPoly(start, extents, &filter, &startRef, startPt);
					if (dtStatusFailed(status) || startRef == 0)
						return ret;

					dtPolyRef endRef;
					float endPt[3];
					status = recast.navMeshQuery->findNearestPoly(end, extents, &filter, &endRef, endPt);
					if (dtStatusFailed(status) || endRef == 0)
						return ret;

					dtPolyRef path[KENGINE_NAVMESH_MAX_PATH_LENGTH];
					int pathCount = 0;
					status = recast.navMeshQuery->findPath(startRef, endRef, startPt, endPt, &filter, path, &pathCount, (int)putils::lengthof(path));
					if (dtStatusFailed(status))
						return ret;

					ret.resize(ret.capacity());
					int straightPathCount = 0;

					static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
					status = recast.navMeshQuery->findStraightPath(startPt, endPt, path, pathCount, ret[0].raw, nullptr, nullptr, &straightPathCount, (int)ret.capacity());
					if (dtStatusFailed(status))
						return ret;

					ret.resize(straightPathCount);
					for (auto & step : ret)
						step = matrixHelper::convertToReferencial(step, modelToWorld);

					return ret;
				};
			}
		};

		return impl::buildNavMeshes();
	}
}
