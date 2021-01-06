#include "luaHelper.hpp"

namespace kengine::luaHelper {
	namespace impl {
		template<typename Func>
		void registerEntityMember(sol::state & state, const char * name, Func && func) noexcept {
			state[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename Func>
		void registerFunctionWithState(sol::state & state, const char * name, Func && func) noexcept {
			state[name] = FWD(func);
		}

		template<typename T>
		void registerTypeWithState(sol::state & state) noexcept {
			putils::lua::registerType<T>(state);
		}

		template<typename T>
		void registerComponentWithState(sol::state & state) noexcept {
			registerTypeWithState<T>(state);
			scriptLanguageHelper::registerComponent<T>(
				[&](auto && ... args) noexcept {
					registerEntityMember(state, FWD(args)...);
				},
				[&](auto && ... args) noexcept {
					registerFunctionWithState(state, FWD(args)...);
				}
			);
		}
	}

	template<typename ...Types>
	void registerTypes() noexcept {
		putils::for_each_type<Types...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			for (const auto & [e, comp] : entities.with<LuaStateComponent>())
				impl::registerTypeWithState<T>(*comp.state);
		});
	}

	template<typename ... Comps>
	void registerComponents() noexcept {
		putils::for_each_type<Comps...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			for (const auto & [e, comp] : entities.with<LuaStateComponent>())
				impl::registerComponentWithState<T>(*comp.state);
		});
	}

	template<typename Func>
	void registerFunction(const char * name, Func && func) noexcept {
		for (const auto & [e, comp] : entities.with<LuaStateComponent>())
			impl::registerFunctionWithState(*comp.state, name, func);
	}
}
