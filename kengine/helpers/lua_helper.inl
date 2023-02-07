#include "lua_helper.hpp"

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/lua/lua_helper.hpp"
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::lua_helper {
	namespace impl {
		template<typename Func>
		void register_entity_member(sol::state & state, const char * name, Func && func) noexcept {
			KENGINE_PROFILING_SCOPE;
			state[putils::reflection::get_class_name<entt::handle>()][name] = FWD(func);
		}

		template<typename Func>
		void register_function_with_state(sol::state & state, const char * name, Func && func) noexcept {
			KENGINE_PROFILING_SCOPE;
			state[name] = FWD(func);
		}

		template<bool IsComponent, typename T>
		void register_type_with_state(entt::registry & r, sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::lua::register_type<T>(state);

			if constexpr (IsComponent) {
				script_language_helper::register_component<T>(
					r,
					[&](auto &&... args) noexcept {
						register_entity_member(state, FWD(args)...);
					},
					[&](auto &&... args) noexcept {
						register_function_with_state(state, FWD(args)...);
					}
				);
			}
		}
	}

	template<bool IsComponent, typename... Types>
	void register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "lua", "Registering types");

		putils::for_each_type<Types...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "lua", "Registering type %s", putils::reflection::get_class_name<type>());
			const auto view = r.view<data::lua_state>();
			std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity e) {
				const putils::scoped_thread_name thread_name(putils::string<128>("Lua registration for %s", putils::reflection::get_class_name<type>()));
				const auto & [comp] = view.get(e);
				impl::register_type_with_state<IsComponent, type>(r, *comp.state);
			});
		});
	}

	template<typename Func>
	void register_function(const entt::registry & r, const char * name, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, log, "lua", "Registering function %s", name);
		for (const auto & [e, comp] : r.view<data::lua_state>().each())
			impl::register_function_with_state(*comp.state, name, func);
	}
}