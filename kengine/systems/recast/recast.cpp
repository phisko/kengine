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
#include "kengine/helpers/new_entity_processor.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// impl
#include "recast_agent.hpp"
#include "recast_crowd.hpp"
#include "recast_nav_mesh.hpp"

namespace kengine::systems {
	namespace recast_impl {
		adjustables g_adjustables;

		void build_recast_component(entt::registry & r, entt::entity e, const data::model & model, const data::model_data & model_data, const data::nav_mesh & nav_mesh) noexcept;
		void process_built_recast_components(entt::registry & r) noexcept;
		void do_pathfinding(entt::registry & r, float delta_time) noexcept;
	}

	struct recast {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, data::model, data::model_data, data::nav_mesh> processor{
			r,
			[this](auto &&... args) noexcept {
				recast_impl::build_recast_component(r, FWD(args)...);
			}
		};

		recast(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "recast", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
			e.emplace<data::adjustable>() = {
				"Recast",
				{
					{ "Path optimization range", &recast_impl::g_adjustables.path_optimization_range },
				}
			};

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Executing");

			processor.process();

			recast_impl::process_built_recast_components(r);
			recast_impl::do_pathfinding(r, delta_time);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		recast,
		recast::processed,
		data::recast_agent,
		data::recast_crowd,
		data::recast_nav_mesh
	)
}