#pragma once

#include "ScriptSystem.hpp"
#include "data/LuaStateComponent.hpp"
#include "lua/plua.hpp"

#include "EntityCreator.hpp"

namespace kengine {
	EntityCreatorFunctor<64> LuaSystem(EntityManager & em);

	namespace lua {
		template<typename Func>
		void registerEntityMember(sol::state & state, const char * name, Func && func) {
			state[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename Func>
		void registerFunctionWithState(sol::state & state, const char * name, Func && func) {
			state[name] = FWD(func);
		}

		template<typename Func>
		void registerFunction(EntityManager & em, const char * name, Func && func) {
			for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
				registerFunctionWithState(state, name, func);
		}

		template<typename T>
		void registerTypeWithState(EntityManager & em, sol::state & state) {
			putils::lua::registerType<T>(state);
			ScriptSystem::registerComponent<T>(
				em,
				[&](auto && ... args) {
					registerEntityMember(state, FWD(args)...);
				},
				[&](auto && ... args) {
					registerFunctionWithState(state, FWD(args)...);
				}
			);
		}

		template<typename T>
		void registerType(EntityManager & em) {
			for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
				registerTypeWithState<T>(em, *comp.state);
		}
    }
}