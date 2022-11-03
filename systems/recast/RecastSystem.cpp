#include "RecastSystem.hpp"
#include "Common.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ShaderComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

// impl
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"

namespace kengine {
	namespace recast {
		Adjustables g_adjustables;

		void buildNavMeshes() noexcept;
		void doPathfinding(float deltaTime) noexcept;
	}

	EntityCreator * RecastSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				kengine_log(Log, "Init", "RecastSystem");

				e += functions::OnEntityRemoved{ onEntityRemoved };
				e += functions::Execute{ execute };

				e += AdjustableComponent{
					"Recast", {
						{ "Path optimization range", &recast::g_adjustables.pathOptimizationRange },
						{ "Editor mode (rebuild navmesh each frame)", &recast::g_adjustables.editorMode }
					}
				};
			}

			static void onEntityRemoved(Entity & e) noexcept {
				const auto agent = e.tryGet<RecastAgentComponent>();
				if (agent) {
					kengine_logf(Log, "RecastSystem", "Removing agent %zu from crowd %zu", e.id, agent->crowd);
					auto environment = entities[agent->crowd];
					environment.get<RecastCrowdComponent>().crowd->removeAgent(agent->index);
				}

				// It doesn't cost us anything to have floating RecastAgentComponents, so we don't remove them when the RecastCrowdComponent is removed
			}

			static void execute(float deltaTime) noexcept {
				kengine_log(Verbose, "Execute", "RecastSystem");
				recast::buildNavMeshes();
				recast::doPathfinding(deltaTime);
			}
		};

		return [](Entity & e) noexcept {
			impl::init(e);
		};
	}
}