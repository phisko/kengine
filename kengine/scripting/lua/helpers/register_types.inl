#include "register_types.hpp"

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/lua/lua_helper.hpp"
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/scripting/helpers/register_component.hpp"
#include "kengine/scripting/lua/data/state.hpp"

#include "log_category.hpp"

namespace kengine::scripting::lua {
	namespace impl {
		template<bool IsComponent, typename T>
		void register_type_with_state(entt::registry & r, sol::state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::lua::register_type<T>(state);

			if constexpr (IsComponent) {
				scripting::register_component<T>(
					r,
					[&](const char * name, auto && func) noexcept {
						state[putils::reflection::get_class_name<entt::handle>()][name] = FWD(func);
					},
					[&](const char * name, auto && func) noexcept {
						state[name] = func;
					}
				);
			}
		}
	}

	template<bool IsComponent, typename... Types>
	void register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, log_category, "Registering types");

		putils::for_each_type<Types...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, log_category, "Registering type %s", putils::reflection::get_class_name<type>());
			const auto view = r.view<state>();
			std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity e) {
				const putils::scoped_thread_name thread_name(putils::string<128>("Lua registration for %s", putils::reflection::get_class_name<type>()));
				const auto & [comp] = view.get(e);
				impl::register_type_with_state<IsComponent, type>(r, *comp.ptr);
			});
		});
	}
}