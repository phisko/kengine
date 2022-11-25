#include "RecastSystem.hpp"
#include "Common.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/NavMeshComponent.hpp"
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

		void buildRecastComponent(entt::registry & r, entt::entity e) noexcept;
		void doPathfinding(entt::registry & r, float deltaTime) noexcept;
	}

	struct RecastSystem {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 5> connections;

		RecastSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "RecastSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<AdjustableComponent>() = {
				"Recast", {
					{"Path optimization range", &recast::g_adjustables.pathOptimizationRange},
					{"Editor mode (rebuild navmesh each frame)", &recast::g_adjustables.editorMode}
				}
			};

			connections.emplace_back(r.on_destroy<RecastAgentComponent>().connect<removeAgentFromCrowds>());
			connections.emplace_back(r.on_construct<ModelComponent>().connect<recast::buildRecastComponent>());
			connections.emplace_back(r.on_construct<ModelDataComponent>().connect<recast::buildRecastComponent>());
			connections.emplace_back(r.on_construct<NavMeshComponent>().connect<recast::buildRecastComponent>());
			connections.emplace_back(r.on_update<NavMeshComponent>().connect<recast::buildRecastComponent>());
		}

		static void removeAgentFromCrowds(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto agent = r.try_get<RecastAgentComponent>(e);
			if (agent) {
				kengine_logf(r, Log, "RecastSystem", "Removing agent %zu from crowd %zu", e, agent->crowd);
				const auto & crowd = r.get<RecastCrowdComponent>(agent->crowd);
				crowd.crowd->removeAgent(agent->index);
			}
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "RecastSystem");
			recast::doPathfinding(r, deltaTime);
		}
	};

	void addRecastSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<RecastSystem>(e);
	}
}