#include "system.hpp"
#include "common.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/pathfinding/data/nav_mesh.hpp"
#include "kengine/pathfinding/recast/data/agent.hpp"
#include "kengine/pathfinding/recast/data/crowd.hpp"
#include "kengine/pathfinding/recast/data/nav_mesh.hpp"

#include "config.hpp"

namespace kengine::pathfinding::recast {
	const config * g_config = nullptr;

	struct system {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, render::model_data, pathfinding::nav_mesh> processor{
			r,
			[this](auto &&... args) noexcept {
				build_recast_component(r, FWD(args)...);
			}
		};

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Recast");
			e.emplace<kengine::config::configurable>();
			g_config = &e.emplace<config>();

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			processor.process();

			process_built_recast_components(r);
			do_pathfinding(r, delta_time);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed,
		agent,
		crowd,
		nav_mesh
	)
}