#include "LuaSystem.hpp"
#include "helpers/luaHelper.hpp"
#include "data/LuaComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine::lua {
	struct impl {
		static inline EntityManager * em;
		static inline sol::state * state;

		static void init(Entity & e) {
			e += functions::Execute{ execute };

			state = new sol::state;
			e += LuaStateComponent{ state };

			state->open_libraries();
			scriptLanguageHelper::init(*em,
				[&](auto && ... args) {
					luaHelper::detail::registerFunctionWithState(*state, FWD(args)...);
				},
				[&](auto type) {
					using T = putils_wrapped_type(type);
					luaHelper::detail::registerTypeWithState<T>(*em, *state);
				}
			);
		}

		static void execute(float deltaTime) {
			(*state)["deltaTime"] = deltaTime;
			for (auto [e, comp] : em->getEntities<LuaComponent>()) {
				(*state)["self"] = &e;
				for (const auto & s : comp.scripts)
					state->script_file(s.c_str());
			}
		}
	};
}

namespace kengine {
	EntityCreatorFunctor<64> LuaSystem(EntityManager & em) {
		lua::impl::em = &em;
		return [&](Entity & e) {
			lua::impl::init(e);
		};
	}
}