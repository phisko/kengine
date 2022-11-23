#include "RecastSystem.hpp"
#include "Common.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ShaderComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"

namespace kengine {
	namespace recast {
		Adjustables g_adjustables;

		void setupNavMeshBuilder(entt::registry & r) noexcept;
		void doPathfinding(entt::registry & r, float deltaTime) noexcept;
	}

	struct RecastSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "RecastSystem");

			_r = &r;

			r.on_destroy<RecastAgentComponent>().connect<removeAgentFromCrowds>();

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			r.emplace<AdjustableComponent>(e) = {
				"Recast", {
					{"Path optimization range", &recast::g_adjustables.pathOptimizationRange},
					{"Editor mode (rebuild navmesh each frame)", &recast::g_adjustables.editorMode}
				}
			};

			recast::setupNavMeshBuilder(r);
		}

		static void removeAgentFromCrowds(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto agent = _r->try_get<RecastAgentComponent>(e);
			if (agent) {
				kengine_logf(*_r, Log, "RecastSystem", "Removing agent %zu from crowd %zu", e, agent->crowd);
				const auto & crowd = _r->get<RecastCrowdComponent>(agent->crowd);
				crowd.crowd->removeAgent(agent->index);
			}
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "RecastSystem");
			recast::doPathfinding(*_r, deltaTime);
		}

		static inline entt::registry * _r;
	};

	void RecastSystem(entt::registry & r) noexcept {
		RecastSystem::init(r);
	}
}