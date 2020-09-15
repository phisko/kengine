#ifndef KENGINE_RECAST_MAX_AGENTS
# define KENGINE_RECAST_MAX_AGENTS 1024
#endif

#include "EntityManager.hpp"

#include "data/ModelComponent.hpp"
#include "data/PathfindingComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "helpers/instanceHelper.hpp"
#include "helpers/matrixHelper.hpp"

#include "Common.hpp"
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"
#include "RecastNavMeshComponent.hpp"

#include "lengthof.hpp"

namespace kengine::recast {
#pragma region declarations
	static void removeOldAgents();
	static void createNewAgents();
	static void moveChangedAgents();
	static void updateCrowds(float deltaTime);
#pragma endregion
	void doPathfinding(float deltaTime) {
		removeOldAgents();
		moveChangedAgents();
		createNewAgents();
		updateCrowds(deltaTime);
	}

	static void removeOldAgents() {
		for (auto & [e, agent, noPathfinding] : g_em->getEntities<RecastAgentComponent, no<PathfindingComponent>>()) {
			auto environment = g_em->getEntity(agent.crowd);
			auto & crowd = environment.get<RecastCrowdComponent>();
			crowd.crowd->removeAgent(agent.index);
			e.detach<RecastAgentComponent>();
		}
	}

#pragma region createNewAgents
#pragma region declarations
	struct EnvironmentInfo {
		putils::Vector3f environmentScale;
		glm::mat4 modelToWorld;
		glm::mat4 worldToModel;
	};
	static EnvironmentInfo getEnvironmentInfo(const Entity & environment);

	struct ObjectInfo {
		putils::Rect3f objectInNavMesh;
		float maxSpeed;
	};
	static ObjectInfo getObjectInfo(const EnvironmentInfo & environment, const TransformComponent & transform, const PathfindingComponent & pathfinding);

	static RecastCrowdComponent & attachCrowdComponent(Entity & e);
	static void attachAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd, Entity::ID crowdId);
#pragma endregion
	static void createNewAgents() {
		for (auto & [e, pathfinding, transform, noRecast] : g_em->getEntities<PathfindingComponent, TransformComponent, no<RecastAgentComponent>>()) {
			if (pathfinding.environment == Entity::INVALID_ID)
				continue;

			auto environment = g_em->getEntity(pathfinding.environment);

			auto crowd = environment.tryGet<RecastCrowdComponent>();
			if (!crowd)
				crowd = &attachCrowdComponent(environment);

			const auto objectInfo = getObjectInfo(getEnvironmentInfo(environment), transform, pathfinding);
			attachAgentComponent(e, objectInfo, *crowd, environment.id);
		}
	}

	static EnvironmentInfo getEnvironmentInfo(const Entity & environment) {
		EnvironmentInfo ret;

		const auto & model = instanceHelper::getModel<ModelComponent>(*g_em, environment);
		const auto & environmentTransform = environment.get<TransformComponent>();

		ret.environmentScale = model.boundingBox.size * environmentTransform.boundingBox.size;
		ret.modelToWorld = matrixHelper::getModelMatrix(model, environmentTransform);
		ret.worldToModel = glm::inverse(ret.modelToWorld);
		return ret;
	}

	static ObjectInfo getObjectInfo(const EnvironmentInfo & environment, const TransformComponent & transform, const PathfindingComponent & pathfinding) {
		ObjectInfo ret;
		ret.objectInNavMesh = {
			matrixHelper::convertToReferencial(transform.boundingBox.position, environment.worldToModel),
			transform.boundingBox.size / environment.environmentScale
		};
		ret.maxSpeed = (putils::Point3f{ pathfinding.maxSpeed, 0.f, 0.f } / environment.environmentScale).getLength();
		return ret;
	}

	static RecastCrowdComponent & attachCrowdComponent(Entity & e) {
		auto & crowd = e.attach<RecastCrowdComponent>();
		crowd.crowd.reset(dtAllocCrowd());

		const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(*g_em, e);
		crowd.crowd->init(KENGINE_RECAST_MAX_AGENTS, navMesh.navMesh->getParams()->tileWidth, navMesh.navMesh.get());

		return crowd;
	}

#pragma region attachAgentComponent
#pragma region declarations
	static void fillCrowdAgentParams(dtCrowdAgentParams & params, const ObjectInfo & objectInfo);
#pragma endregion
	static void attachAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd, Entity::ID crowdId) {
		dtCrowdAgentParams params;
		fillCrowdAgentParams(params, objectInfo);

		params.separationWeight = 0.f;
		params.updateFlags = ~0; // All flags seem to be optimizations, enable them

		params.obstacleAvoidanceType = 0; // Default params, might want to change?
		params.queryFilterType = 0; // Default query type, might want to change?

		params.userData = (void *)e.id;

		const auto idx = crowd.crowd->addAgent(objectInfo.objectInNavMesh.position, &params);
		kengine_assert(*g_em, idx >= 0);

		e += RecastAgentComponent{ idx, crowdId };
	}

	static void fillCrowdAgentParams(dtCrowdAgentParams & params, const ObjectInfo & objectInfo) {
		params.radius = std::max(objectInfo.objectInNavMesh.size.x, objectInfo.objectInNavMesh.size.z);
		params.height = objectInfo.objectInNavMesh.size.y;
		params.maxAcceleration = objectInfo.maxSpeed;
		params.maxSpeed = params.maxAcceleration;

		params.collisionQueryRange = params.radius * 2.f;
		params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.pathOptimizationRange;
	}

#pragma endregion attachAgentComponent
#pragma endregion createNewAgents

	static void moveChangedAgents() {
		for (auto & [e, pathfinding, agent] : g_em->getEntities<PathfindingComponent, RecastAgentComponent>()) {
			if (pathfinding.environment == agent.crowd)
				continue;

			auto oldEnvironment = g_em->getEntity(agent.crowd);
			auto & oldCrowd = oldEnvironment.get<RecastCrowdComponent>();
			oldCrowd.crowd->removeAgent(agent.index);

			auto newEnvironment = g_em->getEntity(pathfinding.environment);
			auto newCrowd = newEnvironment.tryGet<RecastCrowdComponent>();
			if (!newCrowd)
				newCrowd = &attachCrowdComponent(newEnvironment);
			
			const auto objectInfo = getObjectInfo(getEnvironmentInfo(newEnvironment), e.get<TransformComponent>(), pathfinding);
			attachAgentComponent(e, objectInfo, *newCrowd, newEnvironment.id);
		}
	}

#pragma region updateCrowds
#pragma region declarations
	static void readFromAgent(TransformComponent & transform, PhysicsComponent & physics, const dtCrowdAgent & agent, const EnvironmentInfo & environmentInfo);
	static void writeToAgent(Entity & e, const TransformComponent & transform, const PathfindingComponent & pathfinding, const EnvironmentInfo & environmentInfo, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd);
#pragma endregion
	static void updateCrowds(float deltaTime) {
		for (const auto & [environment, crowd, environmentTransform] : g_em->getEntities<RecastCrowdComponent, TransformComponent>()) {
			g_em->runTask([&, environment] {
				const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(*g_em, environment);
				const auto environmentInfo = getEnvironmentInfo(environment);

				static dtCrowdAgent * activeAgents[KENGINE_RECAST_MAX_AGENTS];
				const auto nbAgents = crowd.crowd->getActiveAgents(activeAgents, (int)putils::lengthof(activeAgents));

				// Overwrite agent with user-updated components
				for (int i = 0; i < nbAgents; ++i) {
					const auto agent = activeAgents[i];
					auto e = g_em->getEntity((Entity::ID)agent->params.userData);
					writeToAgent(e, e.get<TransformComponent>(), e.get<PathfindingComponent>(), environmentInfo, navMesh, crowd);
				}

				crowd.crowd->update(deltaTime, nullptr);

				// Update user components with agent info
				for (int i = 0; i < nbAgents; ++i) {
					const auto agent = activeAgents[i];
					auto e = g_em->getEntity((Entity::ID)agent->params.userData);
					readFromAgent(e.get<TransformComponent>(), e.get<PhysicsComponent>(), *agent, environmentInfo);
				}
			});
		}
		g_em->completeTasks();
	}

	static void readFromAgent(TransformComponent & transform, PhysicsComponent & physics, const dtCrowdAgent & agent, const EnvironmentInfo & environmentInfo) {
		physics.movement = environmentInfo.environmentScale * putils::Point3f{ agent.vel };
		transform.boundingBox.position = matrixHelper::convertToReferencial(agent.npos, environmentInfo.modelToWorld);
	}

#pragma region writeToAgent
#pragma region declarations
	static void updateAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd);
	static void updateDestination(Entity & e, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd, const putils::Point3f & destinationInModel, const putils::Point3f & searchExtents);
#pragma endregion
	static void writeToAgent(Entity & e, const TransformComponent & transform, const PathfindingComponent & pathfinding, const EnvironmentInfo & environmentInfo, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd) {
		const auto objectInfo = getObjectInfo(environmentInfo, transform, pathfinding);
		updateAgentComponent(e, objectInfo, crowd);

		const auto destinationInModel = matrixHelper::convertToReferencial(pathfinding.destination, environmentInfo.worldToModel);
		const auto searchExtents = putils::Point3f{ pathfinding.searchDistance, pathfinding.searchDistance, pathfinding.searchDistance } / environmentInfo.environmentScale;
		updateDestination(e, navMesh, crowd, destinationInModel, searchExtents);
	}
	
	static void updateAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd) {
		const auto & agent = e.get<RecastAgentComponent>();
		dtCrowdAgentParams params = crowd.crowd->getAgent(agent.index)->params;
		fillCrowdAgentParams(params, objectInfo);
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
#pragma endregion writeToAgent
#pragma endregion updateCrowds
}