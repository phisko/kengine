#include "luaHelper.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::luaHelper {
	namespace impl {
		template<typename Func>
		void registerEntityMember(sol::state & state, const char * name, Func && func) noexcept {
			KENGINE_PROFILING_SCOPE;
			state[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename Func>
		void registerFunctionWithState(sol::state & state, const char * name, Func && func) noexcept {
			KENGINE_PROFILING_SCOPE;
			state[name] = FWD(func);
		}

		template<typename T>
		void registerTypeWithState(sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;
			putils::lua::registerType<T>(state);
		}

		template<typename T>
		void registerComponentWithState(sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

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
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Types...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			kengine_logf(Log, "Lua/registerTypes", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : entities.with<LuaStateComponent>())
				impl::registerTypeWithState<T>(*comp.state);
		});
	}

	template<typename ... Comps>
	void registerComponents() noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			kengine_logf(Log, "Lua/registerComponents", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : entities.with<LuaStateComponent>())
				impl::registerComponentWithState<T>(*comp.state);
		});
	}

	template<typename Func>
	void registerFunction(const char * name, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(Log, "Lua/registerFunction", "Registering %s", name);
		for (const auto & [e, comp] : entities.with<LuaStateComponent>())
			impl::registerFunctionWithState(*comp.state, name, func);
	}
}