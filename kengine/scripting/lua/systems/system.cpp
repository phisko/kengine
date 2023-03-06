#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/scripting/helpers/init_bindings.hpp"
#include "kengine/scripting/lua/data/scripts.hpp"
#include "kengine/scripting/lua/helpers/log_category.hpp"
#include "kengine/scripting/lua/helpers/register_function.hpp"
#include "kengine/scripting/lua/helpers/register_types.hpp"

namespace kengine::scripting::lua {
	struct system {
		entt::registry & r;
		sol::state * state;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			kengine_log(r, verbose, log_category, "Creating Lua state");
			state = new sol::state;
			e.emplace<lua::state>(state);

			kengine_log(r, verbose, log_category, "Opening libraries");
			state->open_libraries();

			kengine_log(r, verbose, log_category, "Registering script_language_helper functions");
			scripting::init_bindings(
				r,
				[&](const char * name, auto && func) noexcept {
					(*state)[name] = func;
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					impl::register_type_with_state<false, T>(r, *state);
				}
			);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			const auto view = r.view<scripts>();
			if (!view.empty()) {
				kengine_log(r, very_verbose, log_category, "Setting delta_time in Lua state");
				(*state)["delta_time"] = delta_time;
			}

			for (const auto & [e, comp] : view.each()) {
				kengine_logf(r, very_verbose, log_category, "Setting 'self' to {}", e);
				(*state)["self"] = entt::handle{ r, e };

				for (const auto & s : comp.files) {
					kengine_logf(r, very_verbose, log_category, "Running script {} for {}", s, e);

					state->safe_script_file(s.c_str(), [this](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(r, err.what());
						return pfr;
					});
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}