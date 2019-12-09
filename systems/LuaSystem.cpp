#include "LuaSystem.hpp"
#include "ScriptSystem.hpp"
#include "data/LuaComponent.hpp"
#include "functions/Execute.hpp"

#include "meta/type.hpp"

namespace kengine {
	static void execute(EntityManager & em, sol::state & state, float deltaTime);
	EntityCreatorFunctor<64> LuaSystem(EntityManager & em) {
		return [&](Entity & e) {
			auto tmp = std::make_unique<sol::state>();
			auto & state = *tmp;
			e += LuaStateComponent{ std::move(tmp) };

			state.open_libraries();
			ScriptSystem::init(em,
				[&](auto && ... args) {
					lua::registerFunctionWithState(state, FWD(args)...);
				},
				[&](auto type) {
					using T = putils_wrapped_type(type);
					lua::registerTypeWithState<T>(state);
				}
			);

			e += functions::Execute{ [&](float deltaTime) { execute(em, state, deltaTime); } };
		};
	}

	static void execute(EntityManager & em, sol::state & state, float deltaTime) {
		state["deltaTime"] = deltaTime;
		for (auto & [e, comp] : em.getEntities<LuaComponent>()) {
			state["self"] = &e;
			for (const auto & s : comp.scripts)
				state.script_file(s.c_str());
		}
	}
}