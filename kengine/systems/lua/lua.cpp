#include "lua.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/lua.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/lua_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct lua {
		entt::registry & r;
		sol::state * state;

		lua(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "lua", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			kengine_log(r, verbose, "lua", "Creating Lua state");
			state = new sol::state;
			e.emplace<data::lua_state>(state);

			kengine_log(r, verbose, "lua", "Opening libraries");
			state->open_libraries();

			kengine_log(r, verbose, "lua", "Registering script_language_helper functions");
			script_language_helper::init(
				r,
				[&](auto &&... args) noexcept {
					lua_helper::impl::register_function_with_state(*state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					lua_helper::impl::register_type_with_state<false, T>(r, *state);
				}
			);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "lua", "Executing");

			const auto view = r.view<data::lua>();
			if (!view.empty()) {
				kengine_log(r, very_verbose, "lua", "Setting delta_time in Lua state");
				(*state)["delta_time"] = delta_time;
			}

			for (const auto & [e, comp] : view.each()) {
				kengine_logf(r, very_verbose, "lua", "Setting 'self' to [%u]", e);
				(*state)["self"] = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, very_verbose, "lua", "Running script %s for [%u]", s.c_str(), e);

					state->safe_script_file(s.c_str(), [this](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(r, err.what());
						return pfr;
					});
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(lua)
}