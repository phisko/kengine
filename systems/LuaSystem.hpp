#pragma once

#include <string>
#include <functional>

#include "ScriptSystem.hpp"
#include "data/LuaStateComponent.hpp"
#include "lua/plua.hpp"

#include "EntityCreator.hpp"

namespace kengine {
	EntityCreatorFunctor<64> LuaSystem(EntityManager & em);

	namespace lua {
		template<typename Ret, typename ...Args>
		void registerEntityMember(sol::state & state, const std::string & name, const std::function<Ret(Args...)> & func) {
			state[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename T>
		void registerTypeWithState(sol::state & state) {
			putils::lua::registerType<T>(state);
			ScriptSystem::registerComponent<T>([&](auto && ... args) {
				registerEntityMember(state, FWD(args)...);
			});
		}

		template<typename T>
		void registerType(EntityManager & em) {
			for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
				registerTypeWithState<T>(*comp.state);
		}

		template<typename Ret, typename ...Args>
		void registerFunctionWithState(sol::state & state, const std::string & name, const std::function<Ret(Args...)> & func) {
			state[name] = FWD(func);
		}

		template<typename Ret, typename ...Args>
		void registerFunction(EntityManager & em, const std::string & name, const std::function<Ret(Args...)> & func) {
			for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
				registerFunctionWithState(state, name, func);
		}
    }
}