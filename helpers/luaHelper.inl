#include "luaHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::luaHelper {
	namespace impl {
		template<typename Func>
		void registerEntityMember(sol::state & state, const char * name, Func && func) noexcept {
			KENGINE_PROFILING_SCOPE;
			state[putils::reflection::get_class_name<entt::handle>()][name] = FWD(func);
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
		void registerComponentWithState(entt::registry & r, sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			registerTypeWithState<T>(state);
			scriptLanguageHelper::registerComponent<T>(
				r,
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
	void registerTypes(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Types...>([&](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			kengine_logf(r, Log, "Lua/registerTypes", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : r.view<LuaStateComponent>().each())
				impl::registerTypeWithState<T>(*comp.state);
		});
	}

	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			kengine_logf(r, Log, "Lua/registerComponents", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : r.view<LuaStateComponent>().each())
				impl::registerComponentWithState<T>(r, *comp.state);
		});
	}

	template<typename Func>
	void registerFunction(const entt::registry & r, const char * name, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, Log, "Lua/registerFunction", "Registering %s", name);
		for (const auto & [e, comp] : r.view<LuaStateComponent>().each())
			impl::registerFunctionWithState(*comp.state, name, func);
	}
}