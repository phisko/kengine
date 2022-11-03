#include "LuaSystem.hpp"

// kengine data
#include "data/LuaComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/luaHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::lua {
	struct impl {
		static inline sol::state * state;

		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "LuaSystem");

			e += functions::Execute{ execute };

			kengine_log(Log, "Init/LuaSystem", "Creating LuaStateComponent");
			state = new sol::state;
			e += LuaStateComponent{ state };

			kengine_log(Log, "Init/LuaSystem", "Opening libraries");
			state->open_libraries();

			kengine_log(Log, "Init/LuaSystem", "Registering scriptLanguageHelper functions");
			scriptLanguageHelper::init(
				[&](auto && ... args) noexcept {
					luaHelper::impl::registerFunctionWithState(*state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					luaHelper::impl::registerTypeWithState<T>(*state);
				}
			);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "LuaSystem");

			(*state)["deltaTime"] = deltaTime;

			for (auto [e, comp] : entities.with<LuaComponent>()) {
				(*state)["self"] = &e;

				for (const auto & s : comp.scripts) {
					kengine_logf(Verbose, "Execute/LuaSystem", "%zu: %s", e.id, s.c_str());

					state->safe_script_file(s.c_str(), [](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(err.what());
						return pfr;
					});
				}
			}
		}
	};
}

namespace kengine {
	EntityCreator * LuaSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return lua::impl::init;
	}
}