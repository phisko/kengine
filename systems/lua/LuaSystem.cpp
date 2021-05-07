#include "LuaSystem.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/luaHelper.hpp"
#include "data/LuaComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine::lua {
	struct impl {
		static inline sol::state * state;

		static void init(Entity & e) noexcept {
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
			kengine_log(Verbose, "Execute", "LuaSystem");
			(*state)["deltaTime"] = deltaTime;
			for (auto [e, comp] : entities.with<LuaComponent>()) {
				(*state)["self"] = &e;
				for (const auto & s : comp.scripts) {
					kengine_logf(Verbose, "Execute/LuaSystem", "%zu: %s", e.id, s.c_str());
					state->script_file(s.c_str());
				}
			}
		}
	};
}

namespace kengine {
	EntityCreator * LuaSystem() noexcept {
		return [](Entity & e) noexcept {
			lua::impl::init(e);
		};
	}
}