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
#include "helpers/profilingHelper.hpp"

// impl
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"

namespace kengine {
	namespace recast {
		Adjustables g_adjustables;

		void buildNavMeshes() noexcept;
		void doPathfinding(float deltaTime) noexcept;
	}

	struct RecastSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
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
			KENGINE_PROFILING_SCOPE;

			const auto agent = e.tryGet<RecastAgentComponent>();
			if (agent) {
				kengine_logf(Log, "RecastSystem", "Removing agent %zu from crowd %zu", e.id, agent->crowd);
				auto environment = entities[agent->crowd];
				environment.get<RecastCrowdComponent>().crowd->removeAgent(agent->index);
			}
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "RecastSystem");
			recast::buildNavMeshes();
			recast::doPathfinding(deltaTime);
		}
	};

	EntityCreator * RecastSystem() noexcept {
		return RecastSystem::init;
	}
}