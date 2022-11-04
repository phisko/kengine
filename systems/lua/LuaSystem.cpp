#include "LuaSystem.hpp"

// kengine data
#include "data/LuaComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/luaHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct LuaSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "LuaSystem");

			e += functions::Execute{ execute };

			kengine_log(Log, "Init/LuaSystem", "Creating LuaStateComponent");
			_state = new sol::state;
			e += LuaStateComponent{ _state };

			kengine_log(Log, "Init/LuaSystem", "Opening libraries");
			_state->open_libraries();

			kengine_log(Log, "Init/LuaSystem", "Registering scriptLanguageHelper functions");
			scriptLanguageHelper::init(
				[&](auto && ... args) noexcept {
					luaHelper::impl::registerFunctionWithState(*_state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					luaHelper::impl::registerTypeWithState<T>(*_state);
				}
			);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "LuaSystem");

			(*_state)["deltaTime"] = deltaTime;

			for (auto [e, comp] : entities.with<LuaComponent>()) {
				(*_state)["self"] = &e;

				for (const auto & s : comp.scripts) {
					kengine_logf(Verbose, "Execute/LuaSystem", "%zu: %s", e.id, s.c_str());

					_state->safe_script_file(s.c_str(), [](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(err.what());
						return pfr;
					});
				}
			}
		}

		static inline sol::state * _state;
	};
}

namespace kengine {
	EntityCreator * LuaSystem() noexcept {
		return LuaSystem::init;
	}
}