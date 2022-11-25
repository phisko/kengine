#include "recast.hpp"
#include "common.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/model.hpp"
#include "kengine/data/model_data.hpp"
#include "kengine/data/nav_mesh.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// impl
#include "recast_agent.hpp"
#include "recast_crowd.hpp"

namespace kengine::systems {
	namespace recast_impl {
		adjustables g_adjustables;

		void build_recast_component(entt::registry & r, entt::entity e) noexcept;
		void do_pathfinding(entt::registry & r, float delta_time) noexcept;
	}

	struct recast {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 5> connections;

		recast(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/recast");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::adjustable>() = {
				"Recast", {
					{"Path optimization range", &recast_impl::g_adjustables.path_optimization_range},
					{"Editor mode (rebuild navmesh each frame)", &recast_impl::g_adjustables.editor_mode}
				}
			};

			connections.emplace_back(r.on_destroy<data::recast_agent>().connect<remove_agent_from_crowds>());
			connections.emplace_back(r.on_construct<data::model>().connect<recast_impl::build_recast_component>());
			connections.emplace_back(r.on_construct<data::model_data>().connect<recast_impl::build_recast_component>());
			connections.emplace_back(r.on_construct<data::nav_mesh>().connect<recast_impl::build_recast_component>());
			connections.emplace_back(r.on_update<data::nav_mesh>().connect<recast_impl::build_recast_component>());
		}

		static void remove_agent_from_crowds(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto agent = r.try_get<data::recast_agent>(e);
			if (agent) {
				kengine_logf(r, log, "systems/recast", "Removing agent %zu from crowd %zu", e, agent->crowd);
				const auto & crowd = r.get<data::recast_crowd>(agent->crowd);
				crowd.crowd->removeAgent(agent->index);
			}
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "recast");
			recast_impl::do_pathfinding(r, delta_time);
		}
	};

	void add_recast(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<recast>(e);
	}
}