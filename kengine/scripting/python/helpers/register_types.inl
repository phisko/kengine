#include "register_types.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/python/python_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine scripting
#include "kengine/scripting/helpers/register_component.hpp"

// kengine scripting/python
#include "kengine/scripting/python/data/state.hpp"

#include "register_function.hpp"

namespace kengine::scripting::python {
	namespace impl {
		template<typename Ret, typename... Args>
		void register_entity_member(py::class_<entt::handle> & entity, const char * name, const std::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				entity.def(name, func, py::return_value_policy::reference);
			else
				entity.def(name, func);
		}

		template<bool IsComponent, typename T>
		void register_type_with_state(entt::registry & r, state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			const py::gil_scoped_acquire acquire; // In case we're called from a worker thread

			// avoid double registration
			if (hasattr(state.module_, putils::reflection::get_class_name<T>()))
				return;

			if constexpr (std::is_same<T, entt::handle>()) {
				state.entity = new py::class_<entt::handle>(state.module_, putils::reflection::get_class_name<entt::handle>(), py::dynamic_attr());
				putils::reflection::for_each_attribute<entt::handle>([&](const auto & attr) noexcept {
					state.entity->def_readwrite(attr.name, attr.ptr);
				});
				putils::reflection::for_each_method<entt::handle>([&](const auto & attr) noexcept {
					state.entity->def(attr.name, attr.ptr);
				});
			}
			else
				putils::python::register_type<T>(state.module_);

			if constexpr (IsComponent) {
				scripting::register_component<T>(
					r,
					[&](auto &&... args) noexcept {
						register_entity_member(*state.entity, FWD(args)...);
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
		kengine_log(r, verbose, "python", "Registering types");

		putils::for_each_type<Types...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);

			kengine_logf(r, verbose, "python", "Registering type %s", putils::reflection::get_class_name<type>());
			// No point in multithreading this since the GIL can only be owned by one thread
			for (const auto & [e, comp] : r.view<state>().each())
				impl::register_type_with_state<IsComponent, type>(r, comp);
		});
	}
}