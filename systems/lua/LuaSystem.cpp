#include "LuaSystem.hpp"
#include "helpers/luaHelper.hpp"
#include "data/LuaComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine::lua {
	struct impl {
		static inline sol::state * state;

		static void init(Entity & e) noexcept {
			e += functions::Execute{ execute };

			state = new sol::state;
			e += LuaStateComponent{ state };

			state->open_libraries();
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
			(*state)["deltaTime"] = deltaTime;
			for (auto [e, comp] : entities.with<LuaComponent>()) {
				(*state)["self"] = &e;
				for (const auto & s : comp.scripts)
					state->script_file(s.c_str());
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