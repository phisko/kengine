#ifndef KENGINE_RECAST_MAX_AGENTS
# define KENGINE_RECAST_MAX_AGENTS 1024
#endif

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "lengthof.hpp"
#include "on_scope_exit.hpp"

// kengine data
#include "data/PathfindingComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/NavMeshComponent.hpp"

// kengine helpers
#include "helpers/instanceHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "Common.hpp"
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"
#include "RecastNavMeshComponent.hpp"

namespace kengine::recast {
	struct doPathfinding {
		static void run(entt::registry & r, float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (g_adjustables.editorMode)
				recreateCrowds(r);
			removeOldAgents(r);
			moveChangedAgents(r);
			createNewAgents(r);
			updateCrowds(r, deltaTime);
		}

		static void recreateCrowds(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute/RecastSystem", "Recreating crowds for editor mode");

			for (auto [e, agent, transform, pathfinding] : r.view<RecastAgentComponent, TransformComponent, PathfindingComponent>().each()) {
				const auto navMesh = instanceHelper::tryGetModel<NavMeshComponent>({ r, agent.crowd });
				if (!navMesh)
					continue;

				struct NavMeshComponentBackup : NavMeshComponent {};
				const auto backup = r.try_get<NavMeshComponentBackup>(agent.crowd);
				if (backup)
					if (std::memcmp(navMesh, backup, sizeof(*navMesh)) == 0)
						continue;

				r.emplace<NavMeshComponentBackup>(agent.crowd, *navMesh);

				auto & crowd = attachCrowdComponent({ r, agent.crowd });
				const auto objectInfo = getObjectInfo(getEnvironmentInfo({ r, agent.crowd }), transform, pathfinding);
				attachAgentComponent({ r, e }, objectInfo, crowd, agent.crowd);
			}
		}

		static void removeOldAgents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, agent] : r.view<RecastAgentComponent>(entt::exclude<PathfindingComponent>).each()) {
				kengine_logf(r, Verbose, "Execute/RecastSystem", "Removing agent %zu from crowd %zu", e, agent.crowd);
				auto & crowd = r.get<RecastCrowdComponent>(agent.crowd);
				crowd.crowd->removeAgent(agent.index);
				r.remove<RecastAgentComponent>(e);
			}
		}

		static void createNewAgents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, pathfinding, transform] : r.view<PathfindingComponent, TransformComponent>(entt::exclude<RecastAgentComponent>).each()) {
				if (pathfinding.environment == entt::null)
					continue;

				kengine_logf(r, Verbose, "Execute/RecastSystem", "Adding agent %zu to crowd %zu", e, pathfinding.environment);

				auto crowd = r.try_get<RecastCrowdComponent>(pathfinding.environment);
				if (!crowd)
					crowd = &attachCrowdComponent({ r, pathfinding.environment });

				const auto objectInfo = getObjectInfo(getEnvironmentInfo({ r, pathfinding.environment }), transform, pathfinding);
				attachAgentComponent({ r, e }, objectInfo, *crowd, pathfinding.environment);
			}
		}

		struct EnvironmentInfo {
			putils::Vector3f environmentScale;
			glm::mat4 modelToWorld;
			glm::mat4 worldToModel;
		};
		static EnvironmentInfo getEnvironmentInfo(entt::handle environment) noexcept {
			KENGINE_PROFILING_SCOPE;

			EnvironmentInfo ret;

			const auto modelTransform = instanceHelper::tryGetModel<TransformComponent>(environment);
			const auto & environmentTransform = environment.get<TransformComponent>();

			ret.environmentScale = environmentTransform.boundingBox.size;
			if (modelTransform)
				ret.environmentScale *= modelTransform->boundingBox.size;
			ret.modelToWorld = matrixHelper::getModelMatrix(environmentTransform, modelTransform);
			ret.worldToModel = glm::inverse(ret.modelToWorld);
			return ret;
		}

		struct ObjectInfo {
			putils::Rect3f objectInNavMesh;
			float maxSpeed;
		};
		static ObjectInfo getObjectInfo(const EnvironmentInfo & environment, const TransformComponent & transform, const PathfindingComponent & pathfinding) noexcept {
			KENGINE_PROFILING_SCOPE;

			ObjectInfo ret;
			ret.objectInNavMesh = {
				matrixHelper::convertToReferencial(transform.boundingBox.position, environment.worldToModel),
				transform.boundingBox.size / environment.environmentScale
			};
			ret.maxSpeed = putils::getLength(putils::Point3f{ pathfinding.maxSpeed, 0.f, 0.f } / environment.environmentScale);
			return ret;
		}

		static RecastCrowdComponent & attachCrowdComponent(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & crowd = e.emplace<RecastCrowdComponent>();
			crowd.crowd.reset(dtAllocCrowd());

			const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(e);
			crowd.crowd->init(KENGINE_RECAST_MAX_AGENTS, navMesh.navMesh->getParams()->tileWidth, navMesh.navMesh.get());

			return crowd;
		}

		static void attachAgentComponent(entt::handle e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd, entt::entity crowdId) noexcept {
			KENGINE_PROFILING_SCOPE;

			dtCrowdAgentParams params;
			fillCrowdAgentParams(params, objectInfo);

			params.separationWeight = 0.f;
			params.updateFlags = ~0; // All flags seem to be optimizations, enable them

			params.obstacleAvoidanceType = 0; // Default params, might want to change?
			params.queryFilterType = 0; // Default query type, might want to change?

			params.userData = (void *)e.entity();

			const auto idx = crowd.crowd->addAgent(objectInfo.objectInNavMesh.position.raw, &params);
			kengine_assert(*e.registry(), idx >= 0);

			e.emplace<RecastAgentComponent>(idx, crowdId);
		}

		static void fillCrowdAgentParams(dtCrowdAgentParams & params, const ObjectInfo & objectInfo) noexcept {
			KENGINE_PROFILING_SCOPE;

			params.radius = std::max(objectInfo.objectInNavMesh.size.x, objectInfo.objectInNavMesh.size.z);
			params.height = objectInfo.objectInNavMesh.size.y;
			params.maxAcceleration = objectInfo.maxSpeed;
			params.maxSpeed = params.maxAcceleration;

			params.collisionQueryRange = params.radius * 2.f;
			params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.pathOptimizationRange;
		}

		static void moveChangedAgents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [e, pathfinding, agent] : r.view<PathfindingComponent, RecastAgentComponent>().each()) {
				if (pathfinding.environment == agent.crowd)
					continue;

				const auto oldCrowd = r.try_get<RecastCrowdComponent>(agent.crowd);
				if (oldCrowd)
					oldCrowd->crowd->removeAgent(agent.index);

				auto newCrowd = r.try_get<RecastCrowdComponent>(pathfinding.environment);
				if (!newCrowd)
					newCrowd = &attachCrowdComponent({ r, pathfinding.environment });

				const auto objectInfo = getObjectInfo(getEnvironmentInfo({ r, pathfinding.environment }), r.get<TransformComponent>(e), pathfinding);
				attachAgentComponent({ r, e }, objectInfo, *newCrowd, pathfinding.environment);
			}
		}

		static void updateCrowds(entt::registry & r, float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto view = r.view<RecastCrowdComponent>();
			std::for_each(std::execution::par_unseq, view.begin(), view.end(), [&](entt::entity environment) noexcept {
				const auto & [crowd] = view.get(environment);
				updateCrowd(deltaTime, { r, environment }, crowd);
			});
		}

		static void updateCrowd(float deltaTime, entt::handle environment, const RecastCrowdComponent & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(environment);
			const auto environmentInfo = getEnvironmentInfo(environment);

			static dtCrowdAgent * activeAgents[KENGINE_RECAST_MAX_AGENTS];

			const auto nbAgents = crowd.crowd->getActiveAgents(activeAgents, (int)putils::lengthof(activeAgents));

			auto & r = *environment.registry();

			// Overwrite agent with user-updated components
			for (int i = 0; i < nbAgents; ++i) {
				const auto agent = activeAgents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, pathfinding] = r.get<TransformComponent, PathfindingComponent>(e);
				writeToAgent({ r, e }, transform, pathfinding, environmentInfo, navMesh, crowd);
			}

			crowd.crowd->update(deltaTime, nullptr);

			// Update user components with agent info
			for (int i = 0; i < nbAgents; ++i) {
				const auto agent = activeAgents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, physics] = r.get<TransformComponent, PhysicsComponent>(e);
				readFromAgent(transform, physics, *agent, environmentInfo);
			}
		}

		static void readFromAgent(TransformComponent & transform, PhysicsComponent & physics, const dtCrowdAgent & agent, const EnvironmentInfo & environmentInfo) noexcept {
			KENGINE_PROFILING_SCOPE;

			physics.movement = environmentInfo.environmentScale * putils::Point3f{ agent.vel };
			transform.boundingBox.position = matrixHelper::convertToReferencial(agent.npos, environmentInfo.modelToWorld);
		}

		static void writeToAgent(entt::handle e, const TransformComponent & transform, const PathfindingComponent & pathfinding, const EnvironmentInfo & environmentInfo, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto objectInfo = getObjectInfo(environmentInfo, transform, pathfinding);
			updateAgentComponent(e, objectInfo, crowd);

			const auto destinationInModel = matrixHelper::convertToReferencial(pathfinding.destination, environmentInfo.worldToModel);
			const auto searchExtents = putils::Point3f{ pathfinding.searchDistance, pathfinding.searchDistance, pathfinding.searchDistance } / environmentInfo.environmentScale;
			updateDestination(e, navMesh, crowd, destinationInModel, searchExtents);
		}

		static void updateAgentComponent(entt::handle e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & agent = e.get<RecastAgentComponent>();
			const auto editableAgent = crowd.crowd->getEditableAgent(agent.index);
			fillCrowdAgentParams(editableAgent->params, objectInfo);
			memcpy(editableAgent->npos, objectInfo.objectInNavMesh.position.raw, sizeof(float[3]));
			memcpy(editableAgent->nvel, e.get<PhysicsComponent>().movement.raw, sizeof(float[3]));
		}

		static void updateDestination(entt::handle e, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd, const putils::Point3f & destinationInModel, const putils::Point3f & searchExtents) noexcept {
			KENGINE_PROFILING_SCOPE;

			static const dtQueryFilter filter;
			dtPolyRef nearestPoly;
			float nearestPt[3];
			const auto status = navMesh.navMeshQuery->findNearestPoly(destinationInModel.raw, searchExtents.raw, &filter, &nearestPoly, nearestPt);
			if (dtStatusFailed(status) || nearestPoly == 0)
				return;

			const auto & agent = e.get<RecastAgentComponent>();
			if (!crowd.crowd->requestMoveTarget(agent.index, nearestPoly, nearestPt))
				kengine_assert_failed(*e.registry(), "[Recast] Failed to request move");
		}
	};

	void doPathfinding(entt::registry & r, float deltaTime) noexcept {
		doPathfinding::run(r, deltaTime);
	}
}