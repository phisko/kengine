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
			kengine_log(r, log, "Init", "systems/lua");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			kengine_log(r, log, "Init/systems/lua", "Creating data::lua_state");
			state = new sol::state;
			e.emplace<data::lua_state>(state);

			kengine_log(r, log, "Init/systems/lua", "Opening libraries");
			state->open_libraries();

			kengine_log(r, log, "Init/systems/lua", "Registering script_language_helper functions");
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
			kengine_log(r, verbose, "execute", "lua");

			(*state)["delta_time"] = delta_time;

			for (auto [e, comp] : r.view<data::lua>().each()) {
				(*state)["self"] = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, verbose, "execute/lua", "%zu: %s", e, s.c_str());

					state->safe_script_file(s.c_str(), [this](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(r, err.what());
						return pfr;
					});
				}
			}
		}
	};

	entt::entity add_lua(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<lua>(e);
		return e;
	}
}