#ifndef KENGINE_RECAST_MAX_AGENTS
# define KENGINE_RECAST_MAX_AGENTS 1024
#endif

#include "data/PathfindingComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/NavMeshComponent.hpp"

#include "helpers/instanceHelper.hpp"
#include "helpers/matrixHelper.hpp"

#include "Common.hpp"
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"
#include "RecastNavMeshComponent.hpp"

#include "lengthof.hpp"
#include "on_scope_exit.hpp"

namespace kengine::recast {
	void doPathfinding(float deltaTime) noexcept {
		struct impl {
			static void doPathfinding(float deltaTime) noexcept {
				if (g_adjustables.editorMode)
					recreateCrowds();
				removeOldAgents();
				moveChangedAgents();
				createNewAgents();
				updateCrowds(deltaTime);
			}

			static void recreateCrowds() noexcept {
				for (auto [e, agent, transform, pathfinding] : entities.with<RecastAgentComponent, TransformComponent, PathfindingComponent>()) {
					auto environment = entities[agent.crowd];
					const auto navMesh = instanceHelper::tryGetModel<NavMeshComponent>(environment);
					if (!navMesh)
						continue;

					struct NavMeshComponentBackup : NavMeshComponent {};
					const auto backup = environment.tryGet<NavMeshComponentBackup>();
					if (backup)
						if (std::memcmp(navMesh, backup, sizeof(*navMesh)) == 0)
							continue;

					environment += NavMeshComponentBackup{ *navMesh };

					auto & crowd = attachCrowdComponent(environment);
					const auto objectInfo = getObjectInfo(getEnvironmentInfo(environment), transform, pathfinding);
					attachAgentComponent(e, objectInfo, crowd, environment.id);
				}
			}

			static void removeOldAgents() noexcept {
				for (auto [e, agent, noPathfinding] : entities.with<RecastAgentComponent, no<PathfindingComponent>>()) {
					auto environment = entities[agent.crowd];
					auto & crowd = environment.get<RecastCrowdComponent>();
					crowd.crowd->removeAgent(agent.index);
					e.detach<RecastAgentComponent>();
				}
			}

			static void createNewAgents() noexcept {
				for (auto [e, pathfinding, transform, noRecast] : entities.with<PathfindingComponent, TransformComponent, no<RecastAgentComponent>>()) {
					if (pathfinding.environment == INVALID_ID)
						continue;

					auto environment = entities[pathfinding.environment];

					auto crowd = environment.tryGet<RecastCrowdComponent>();
					if (!crowd)
						crowd = &attachCrowdComponent(environment);

					const auto objectInfo = getObjectInfo(getEnvironmentInfo(environment), transform, pathfinding);
					attachAgentComponent(e, objectInfo, *crowd, environment.id);
				}
			}

			struct EnvironmentInfo {
				putils::Vector3f environmentScale;
				glm::mat4 modelToWorld;
				glm::mat4 worldToModel;
			};
			static EnvironmentInfo getEnvironmentInfo(const Entity & environment) noexcept {
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
				ObjectInfo ret;
				ret.objectInNavMesh = {
					matrixHelper::convertToReferencial(transform.boundingBox.position, environment.worldToModel),
					transform.boundingBox.size / environment.environmentScale
				};
				ret.maxSpeed = putils::getLength(putils::Point3f{ pathfinding.maxSpeed, 0.f, 0.f } / environment.environmentScale);
				return ret;
			}

			static RecastCrowdComponent & attachCrowdComponent(Entity & e) noexcept {
				auto & crowd = e.attach<RecastCrowdComponent>();
				crowd.crowd.reset(dtAllocCrowd());

				const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(e);
				crowd.crowd->init(KENGINE_RECAST_MAX_AGENTS, navMesh.navMesh->getParams()->tileWidth, navMesh.navMesh.get());

				return crowd;
			}

			static void attachAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd, EntityID crowdId) noexcept {
				dtCrowdAgentParams params;
				fillCrowdAgentParams(params, objectInfo);

				params.separationWeight = 0.f;
				params.updateFlags = ~0; // All flags seem to be optimizations, enable them

				params.obstacleAvoidanceType = 0; // Default params, might want to change?
				params.queryFilterType = 0; // Default query type, might want to change?

				params.userData = (void *)e.id;

				const auto idx = crowd.crowd->addAgent(objectInfo.objectInNavMesh.position, &params);
				kengine_assert(idx >= 0);

				e += RecastAgentComponent{ idx, crowdId };
			}

			static void fillCrowdAgentParams(dtCrowdAgentParams & params, const ObjectInfo & objectInfo) noexcept {
				params.radius = std::max(objectInfo.objectInNavMesh.size.x, objectInfo.objectInNavMesh.size.z);
				params.height = objectInfo.objectInNavMesh.size.y;
				params.maxAcceleration = objectInfo.maxSpeed;
				params.maxSpeed = params.maxAcceleration;

				params.collisionQueryRange = params.radius * 2.f;
				params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.pathOptimizationRange;
			}

			static void moveChangedAgents() noexcept {
				for (auto [e, pathfinding, agent] : entities.with<PathfindingComponent, RecastAgentComponent>()) {
					if (pathfinding.environment == agent.crowd)
						continue;

					auto oldEnvironment = entities[agent.crowd];
					const auto oldCrowd = oldEnvironment.tryGet<RecastCrowdComponent>();
					if (oldCrowd)
						oldCrowd->crowd->removeAgent(agent.index);

					auto newEnvironment = entities[pathfinding.environment];
					auto newCrowd = newEnvironment.tryGet<RecastCrowdComponent>();
					if (!newCrowd)
						newCrowd = &attachCrowdComponent(newEnvironment);

					const auto objectInfo = getObjectInfo(getEnvironmentInfo(newEnvironment), e.get<TransformComponent>(), pathfinding);
					attachAgentComponent(e, objectInfo, *newCrowd, newEnvironment.id);
				}
			}

			static void updateCrowds(float deltaTime) noexcept {
				std::atomic<size_t> jobsLeft = 0;

				for (const auto & [environment, crowd, environmentTransform] : entities.with<RecastCrowdComponent, TransformComponent>()) {
					++jobsLeft;
					threadPool().runTask([&, environment]() noexcept {
						const auto cleanup = putils::onScopeExit([&] { --jobsLeft; });
						const auto & navMesh = instanceHelper::getModel<RecastNavMeshComponent>(environment);
						const auto environmentInfo = getEnvironmentInfo(environment);

						static dtCrowdAgent * activeAgents[KENGINE_RECAST_MAX_AGENTS];
						const auto nbAgents = crowd.crowd->getActiveAgents(activeAgents, (int)putils::lengthof(activeAgents));

						// Overwrite agent with user-updated components
						for (int i = 0; i < nbAgents; ++i) {
							const auto agent = activeAgents[i];
							auto e = entities[(EntityID)agent->params.userData];
							writeToAgent(e, e.get<TransformComponent>(), e.get<PathfindingComponent>(), environmentInfo, navMesh, crowd);
						}

						crowd.crowd->update(deltaTime, nullptr);

						// Update user components with agent info
						for (int i = 0; i < nbAgents; ++i) {
							const auto agent = activeAgents[i];
							auto e = entities[(EntityID)agent->params.userData];
							readFromAgent(e.get<TransformComponent>(), e.get<PhysicsComponent>(), *agent, environmentInfo);
						}
					});
				}

				while (jobsLeft > 0);
			}

			static void readFromAgent(TransformComponent & transform, PhysicsComponent & physics, const dtCrowdAgent & agent, const EnvironmentInfo & environmentInfo) noexcept {
				physics.movement = environmentInfo.environmentScale * putils::Point3f{ agent.vel };
				transform.boundingBox.position = matrixHelper::convertToReferencial(agent.npos, environmentInfo.modelToWorld);
			}

			static void writeToAgent(Entity & e, const TransformComponent & transform, const PathfindingComponent & pathfinding, const EnvironmentInfo & environmentInfo, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd) noexcept {
				const auto objectInfo = getObjectInfo(environmentInfo, transform, pathfinding);
				updateAgentComponent(e, objectInfo, crowd);

				const auto destinationInModel = matrixHelper::convertToReferencial(pathfinding.destination, environmentInfo.worldToModel);
				const auto searchExtents = putils::Point3f{ pathfinding.searchDistance, pathfinding.searchDistance, pathfinding.searchDistance } / environmentInfo.environmentScale;
				updateDestination(e, navMesh, crowd, destinationInModel, searchExtents);
			}

			static void updateAgentComponent(Entity & e, const ObjectInfo & objectInfo, const RecastCrowdComponent & crowd) noexcept {
				const auto & agent = e.get<RecastAgentComponent>();
				const auto editableAgent = crowd.crowd->getEditableAgent(agent.index);
				fillCrowdAgentParams(editableAgent->params, objectInfo);
				memcpy(editableAgent->npos, objectInfo.objectInNavMesh.position, sizeof(float[3]));
				memcpy(editableAgent->nvel, e.get<PhysicsComponent>().movement, sizeof(float[3]));
			}

			static void updateDestination(Entity & e, const RecastNavMeshComponent & navMesh, const RecastCrowdComponent & crowd, const putils::Point3f & destinationInModel, const putils::Point3f & searchExtents) noexcept {
				static const dtQueryFilter filter;
				dtPolyRef nearestPoly;
				float nearestPt[3];
				const auto status = navMesh.navMeshQuery->findNearestPoly(destinationInModel, searchExtents, &filter, &nearestPoly, nearestPt);
				if (dtStatusFailed(status) || nearestPoly == 0)
					return;

				const auto & agent = e.get<RecastAgentComponent>();
				if (!crowd.crowd->requestMoveTarget(agent.index, nearestPoly, nearestPt))
					kengine_assert_failed("[Recast] Failed to request move");
			}
		};

		impl::doPathfinding(deltaTime);
	}
}