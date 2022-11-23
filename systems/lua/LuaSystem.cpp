#include "LuaSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "LuaSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			kengine_log(r, Log, "Init/LuaSystem", "Creating LuaStateComponent");
			_state = new sol::state;
			r.emplace<LuaStateComponent>(e, _state);

			kengine_log(r, Log, "Init/LuaSystem", "Opening libraries");
			_state->open_libraries();

			kengine_log(r, Log, "Init/LuaSystem", "Registering scriptLanguageHelper functions");
			scriptLanguageHelper::init(
				r,
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
			kengine_log(*_r, Verbose, "Execute", "LuaSystem");

			(*_state)["deltaTime"] = deltaTime;

			for (auto [e, comp] : _r->view<LuaComponent>().each()) {
				(*_state)["self"] = entt::handle{ *_r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(*_r, Verbose, "Execute/LuaSystem", "%zu: %s", e, s.c_str());

					_state->safe_script_file(s.c_str(), [](lua_State *, sol::protected_function_result pfr) {
						const sol::error err = pfr;
						kengine_assert_failed(*_r, err.what());
						return pfr;
					});
				}
			}
		}

		static inline entt::registry * _r;
		static inline sol::state * _state;
	};
}

namespace kengine {
	void LuaSystem(entt::registry & r) noexcept {
		LuaSystem::init(r);
	}
}