#pragma once

#include "ScriptLanguageHelper.hpp"
#include "data/LuaStateComponent.hpp"
#include "lua_helper.hpp"

namespace kengine::luaHelper {
	template<typename T>
	void registerType(EntityManager & em);
	template<typename ... Types>
	void registerTypes(EntityManager & em);

	template<typename Comp>
	void registerComponent(EntityManager & em);
	template<typename ... Comps>
	void registerComponents(EntityManager & em);

	template<typename Func>
	void registerFunction(EntityManager & em, const char * name, Func && func);
}

// Impl
namespace kengine::luaHelper {
	namespace detail {
		template<typename Func>
		void registerEntityMember(sol::state & state, const char * name, Func && func) {
			state[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename Func>
		void registerFunctionWithState(sol::state & state, const char * name, Func && func) {
			state[name] = FWD(func);
		}

		template<typename T>
		void registerTypeWithState(EntityManager & em, sol::state & state) {
			putils::lua::registerType<T>(state);
		}

		template<typename T>
		void registerComponentWithState(EntityManager & em, sol::state & state) {
			registerTypeWithState<T>(em, state);
			scriptLanguageHelper::registerComponent<T>(
				em,
				[&](auto && ... args) {
					registerEntityMember(state, FWD(args)...);
				},
				[&](auto && ... args) {
					registerFunctionWithState(state, FWD(args)...);
				}
			);
		}
	}

	template<typename T>
	void registerType(EntityManager & em) {
		for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
			detail::registerTypeWithState<T>(em, *comp.state);
	}

	template<typename ...Types>
	void registerTypes(EntityManager & em) {
		putils::for_each_type<Types...>([&](auto && t) {
			using T = putils_wrapped_type(t);
			registerType<T>(em);
		});
	}

	template<typename Comp>
	void registerComponent(EntityManager & em) {
		for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
			detail::registerComponentWithState<Comp>(em, *comp.state);
	}

	template<typename ... Comps>
	void registerComponents(EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto && t) {
			using T = putils_wrapped_type(t);
			registerComponent<T>(em);
		});
	}

	template<typename Func>
	void registerFunction(EntityManager & em, const char * name, Func && func) {
		for (const auto & [e, comp] : em.getEntities<LuaStateComponent>())
			detail::registerFunctionWithState(state, name, func);
	}
}
