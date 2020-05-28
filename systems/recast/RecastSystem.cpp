#ifndef KENGINE_RECAST_MAX_AGENTS
# define KENGINE_RECAST_MAX_AGENTS 1024
#endif

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
#include "data/PathfindingComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"

#include "helpers/assertHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"

#include "angle.hpp"
#include "with.hpp"

namespace kengine {
	static EntityManager * g_em;

	namespace Flags {
		enum {
			Walk = 1,
		};
	}

	struct {
		float pathOptimizationRange = 2.f;
	} g_adjustables;

#pragma region RecastSystem
#pragma region declarations
	static void onEntityRemoved(Entity & e);
	static void execute(float deltaTime);
#pragma endregion
	//
	EntityCreator * RecastSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::Execute{ execute };
			e += makeGBufferShaderComponent<RecastDebugShader>(*g_em);

			e += AdjustableComponent{
				"Recast", {
					{ "[Recast] Path optimization range", &g_adjustables.pathOptimizationRange }
				}
			};
		};
	}

	static void onEntityRemoved(Entity & e) {
		if (e.has<RecastAgentComponent>()) {
			const auto & agent = e.get<RecastAgentComponent>();
			auto environment = g_em->getEntity(agent.crowd);
			environment.get<RecastCrowdComponent>().crowd->removeAgent(agent.index);
		}

		// It doesn't cost us anything to have floating RecastAgentComponents, so we don't remove them when the RecastCrowdComponent is removed
	}

#pragma region execute
#pragma region declarations
	static void buildNavMeshes();
	static void doPathfinding(float deltaTime);
#pragma endregion
	static void execute(float deltaTime) {
		buildNavMeshes();
		doPathfinding(deltaTime);
	}

#pragma region doPathfinding
#pragma region declarations
	static putils::Point3f convertPosToReferencial(const putils::Point3f & pos, const glm::mat4 & worldToModel);
	static void attachCrowdComponent(Entity & e, const RecastNavMeshComponent & navMesh);
	static void attachAgentComponent(Entity & e, const putils::Rect3f & objectInNavMesh, const RecastCrowdComponent & crowd, Entity::ID crowdId, float maxAcceleration);
	static void updateAgentComponent(Entity & e, const putils::Rect3f & objectInNavMesh, const RecastCrowdComponent & crowd, Entity::ID crowdId, float maxAcceleration);
	static void updateDestination(Entity & e, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd, const putils::Point3f & destinationInModel, const putils::Point3f & searchExtents);
#pragma endregion declarations
	static void doPathfinding(float deltaTime) {
		for (auto & [e, pathfinding, transform, physics] : g_em->getEntities<PathfindingComponent, TransformComponent, PhysicsComponent>()) {
			if (pathfinding.environment == Entity::INVALID_ID)
				continue;

			auto environment = g_em->getEntity(pathfinding.environment);
			const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(*g_em, environment);

			if (!environment.has<RecastCrowdComponent>())
				attachCrowdComponent(environment, navMesh);

			auto & crowd = environment.get<RecastCrowdComponent>();

			const auto & model = instanceHelper::getModel<ModelComponent>(*g_em, environment);
			const auto & environmentTransform = environment.get<TransformComponent>();
			const auto environmentScale = model.boundingBox.size * environmentTransform.boundingBox.size;

			const auto modelToWorld = shaderHelper::getModelMatrix(model, environmentTransform);
			const auto worldToModel = glm::inverse(modelToWorld);

			const putils::Rect3f objectInNavMesh = {
				convertPosToReferencial(transform.boundingBox.position, worldToModel),
				transform.boundingBox.size / environmentScale
			};

			// const auto maxSpeed = (physics.movement / environmentScale).getLength();
			const auto maxSpeed = (putils::Point3f{ 1.f, 1.f, 1.f } / environmentScale).getLength();
			if (!e.has<RecastAgentComponent>())
				attachAgentComponent(e, objectInNavMesh, crowd, environment.id, maxSpeed);
			else
				updateAgentComponent(e, objectInNavMesh, crowd, environment.id, maxSpeed);

			const auto destinationInModel = convertPosToReferencial(pathfinding.destination, worldToModel);
			const auto searchExtents = putils::Point3f{ pathfinding.searchDistance, pathfinding.searchDistance, pathfinding.searchDistance } / environmentScale;
			updateDestination(e, navMesh, crowd, destinationInModel, searchExtents);
		}

		for (const auto & [environment, crowd, environmentTransform] : g_em->getEntities<RecastCrowdComponent, TransformComponent>()) {
			crowd.crowd->update(deltaTime, nullptr);

			const auto & model = instanceHelper::getModel<ModelComponent>(*g_em, environment);
			const auto environmentScale = model.boundingBox.size * environmentTransform.boundingBox.size;

			const auto modelToWorld = shaderHelper::getModelMatrix(model, environmentTransform);

			static dtCrowdAgent * activeAgents[KENGINE_RECAST_MAX_AGENTS];
			const auto nbAgents = crowd.crowd->getActiveAgents(activeAgents, lengthof(activeAgents));
			for (int i = 0; i < nbAgents; ++i) {
				const auto agent = activeAgents[i];

				auto e = g_em->getEntity((Entity::ID)agent->params.userData);

				auto & physics = e.get<PhysicsComponent>();
				physics.movement = environmentScale * putils::Point3f{ agent->vel };

				auto & transform = e.get<TransformComponent>();
				transform.boundingBox.position = convertPosToReferencial(agent->npos, modelToWorld);
			}
		}
	}

	static putils::Point3f convertPosToReferencial(const putils::Point3f & pos, const glm::mat4 & worldToModel) {
		const auto tmp = worldToModel * glm::vec4(shaderHelper::toVec(pos), 1.f);
		return { tmp.x, tmp.y, tmp.z };
	}

	static void attachCrowdComponent(Entity & e, const RecastNavMeshComponent & navMesh) {
		auto & crowd = e.attach<RecastCrowdComponent>();
		crowd.crowd.reset(dtAllocCrowd());
		crowd.crowd->init(KENGINE_RECAST_MAX_AGENTS, navMesh.navMesh->getParams()->tileWidth, navMesh.navMesh.get());
	}

	static void attachAgentComponent(Entity & e, const putils::Rect3f & objectInNavMesh, const RecastCrowdComponent & crowd, Entity::ID crowdId, float maxAcceleration) {
		dtCrowdAgentParams params;

		params.radius = std::max(objectInNavMesh.size.x, objectInNavMesh.size.z);
		params.height = objectInNavMesh.size.y;
		params.maxAcceleration = maxAcceleration;
		params.maxSpeed = params.maxAcceleration;

		params.collisionQueryRange = params.radius * 2.f;
		params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.pathOptimizationRange;

		params.separationWeight = 0.f;
		params.updateFlags = ~0; // All flags seem to be optimizations, enable them

		params.obstacleAvoidanceType = 0; // Default params, might want to change?
		params.queryFilterType = 0; // Default query type, might want to change?

		params.userData = (void *)e.id;

		const auto idx = crowd.crowd->addAgent(objectInNavMesh.position, &params);
		kengine_assert(*g_em, idx >= 0);

		e += RecastAgentComponent{ idx, crowdId };
	}

	static void updateAgentComponent(Entity & e, const putils::Rect3f & objectInNavMesh, const RecastCrowdComponent & crowd, Entity::ID crowdId, float maxAcceleration) {
		const auto & agent = e.get<RecastAgentComponent>();

		dtCrowdAgentParams params = crowd.crowd->getAgent(agent.index)->params;

		params.radius = std::max(objectInNavMesh.size.x, objectInNavMesh.size.z);
		params.height = objectInNavMesh.size.y;
		params.maxAcceleration = maxAcceleration;
		params.maxSpeed = params.maxAcceleration;

		params.collisionQueryRange = params.radius * 2.f;
		params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.pathOptimizationRange;

		crowd.crowd->updateAgentParameters(agent.index, &params);
	}

	static void updateDestination(Entity & e, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd, const putils::Point3f & destinationInModel, const putils::Point3f & searchExtents) {
		static const dtQueryFilter filter;
		dtPolyRef nearestPoly;
		float nearestPt[3];
		const auto status = navMesh.navMeshQuery->findNearestPoly(destinationInModel, searchExtents, &filter, &nearestPoly, nearestPt);
		if (dtStatusFailed(status) || nearestPoly == 0)
			return;

		const auto & agent = e.get<RecastAgentComponent>();
		if (!crowd.crowd->requestMoveTarget(agent.index, nearestPoly, nearestPt))
			kengine_assert_failed(*g_em, "[Recast] Failed to request move");
	}
#pragma endregion doPathfinding

#pragma region buildNavMeshes
#pragma region declarations
	static void createRecastMesh(const char * file, Entity & model, NavMeshComponent & navMesh, const ModelDataComponent & modelData);
#pragma endregion
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

		buildRecastComponent(g_em->getEntities<ModelComponent, ModelDataComponent, NavMeshComponent, no<RecastNavMeshComponent>>());
		buildRecastComponent(g_em->getEntities<ModelComponent, ModelDataComponent, NavMeshComponent, RebuildNavMeshComponent>());
	}

#pragma region createRecastMesh
	// declarations
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

	static NavMeshData loadBinaryFile(const char * binaryFile, const NavMeshComponent & navMesh);
	static void saveBinaryFile(const char * binaryFile, const NavMeshData & data, const NavMeshComponent & navMesh);
	static NavMeshData createNavMeshData(const NavMeshComponent & navMesh, const ModelDataComponent & modelData, const ModelDataComponent::Mesh & meshData);
	static NavMeshPtr createNavMesh(const NavMeshData & data);
	static NavMeshQueryPtr createNavMeshQuery(const NavMeshComponent & params, const dtNavMesh & navMesh);
	static NavMeshComponent::GetPathFunc getPath(const ModelComponent & model, const NavMeshComponent & navMesh, const RecastNavMeshComponent & recast);
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

#pragma region createNavMeshData
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
			kengine_assert_failed(*g_em, "[Recast] Failed to create Detour navmesh data");

		ret.areaSize = (putils::Point3f(cfg.bmax) - putils::Point3f(cfg.bmin)).getLength();
		return ret;
	}

#pragma region getVertices
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
#pragma endregion getVertices

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
#pragma endregion createNavMeshData

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

#pragma region getPath
	// declarations
	static putils::Point3f getPositionInModelSpace(const putils::Point3f & pos, const glm::mat4 & worldToModel);
	static void convertToWorldSpace(NavMeshComponent::Path & path, const glm::mat4 & modelToWorld);
	//
	static NavMeshComponent::GetPathFunc getPath(const ModelComponent & model, const NavMeshComponent & navMesh, const RecastNavMeshComponent & recast) {
		return [&](const Entity & environment, const putils::Point3f & startWorldSpace, const putils::Point3f & endWorldSpace) {
			static const dtQueryFilter filter;

			const auto modelToWorld = shaderHelper::getModelMatrix(model, environment.get<TransformComponent>());
			const auto worldToModel = glm::inverse(modelToWorld);

			const auto start = getPositionInModelSpace(startWorldSpace, worldToModel);
			const auto end = getPositionInModelSpace(endWorldSpace, worldToModel);

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
			status = recast.navMeshQuery->findPath(startRef, endRef, startPt, endPt, &filter, path, &pathCount, lengthof(path));
			if (dtStatusFailed(status))
				return ret;

			ret.resize(ret.capacity());
			int straightPathCount = 0;

			static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
			status = recast.navMeshQuery->findStraightPath(startPt, endPt, path, pathCount, ret[0].raw, nullptr, nullptr, &straightPathCount, (int)ret.capacity());
			if (dtStatusFailed(status))
				return ret;

			ret.resize(straightPathCount);
			convertToWorldSpace(ret, modelToWorld);

			return ret;
		};
	}

	static putils::Point3f getPositionInModelSpace(const putils::Point3f & pos, const glm::mat4 & worldToModel) {
		auto tmp = glm::vec4(shaderHelper::toVec(pos), 1.f);
		tmp = worldToModel * tmp;
		return { tmp.x, tmp.y, tmp.z };
	}

	static void convertToWorldSpace(NavMeshComponent::Path & path, const glm::mat4 & modelToWorld) {
		for (auto & step : path) { // Convert positions to world space
			glm::vec4 v(shaderHelper::toVec(step), 1.f);
			v = modelToWorld * v;
			step = { v.x, v.y, v.z };
		}
	}
#pragma endregion getPath

#pragma endregion createRecastMesh

#pragma endregion buildNavMeshes

#pragma endregion execute

#pragma endregion RecastSystem
}