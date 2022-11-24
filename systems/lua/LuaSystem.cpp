#include "LuaSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/LuaComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/luaHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct LuaSystem {
		entt::registry & r;
		sol::state * state;

		LuaSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "LuaSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			kengine_log(r, Log, "Init/LuaSystem", "Creating LuaStateComponent");
			state = new sol::state;
			e.emplace<LuaStateComponent>(state);

			kengine_log(r, Log, "Init/LuaSystem", "Opening libraries");
			state->open_libraries();

			kengine_log(r, Log, "Init/LuaSystem", "Registering scriptLanguageHelper functions");
			scriptLanguageHelper::init(
				r,
				[&](auto && ... args) noexcept {
					luaHelper::impl::registerFunctionWithState(*state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					luaHelper::impl::registerTypeWithState<T>(*state);
				}
			);
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "LuaSystem");

			(*state)["deltaTime"] = deltaTime;

			for (auto [e, comp] : r.view<LuaComponent>().each()) {
				(*state)["self"] = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, Verbose, "Execute/LuaSystem", "%zu: %s", e, s.c_str());

					state->safe_script_file(s.c_str(), [this](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(r, err.what());
						return pfr;
					});
				}
			}
		}
	};

	void addLuaSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<LuaSystem>(e);
	}
}