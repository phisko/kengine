#include "python_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/python/python_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine scripting/python
#include "kengine/scripting/python/data/python_state.hpp"

namespace kengine::python_helper {
	namespace impl {
		template<typename Ret, typename... Args>
		void register_entity_member(py::class_<entt::handle> & entity, const char * name, const script_language_helper::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				entity.def(name, func, py::return_value_policy::reference);
			else
				entity.def(name, func);
		}

		template<typename Ret, typename... Args>
		void register_function_with_state(data::python_state & state, const char * name, const script_language_helper::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				state.module_.def(name, func, py::return_value_policy::reference);
			else
				state.module_.def(name, func);
		}

		template<bool IsComponent, typename T>
		void register_type_with_state(entt::registry & r, data::python_state & state) noexcept {
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
				script_language_helper::register_component<T>(
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
			for (const auto & [e, comp] : r.view<data::python_state>().each())
				impl::register_type_with_state<IsComponent, type>(r, comp);
		});
	}

	template<typename Ret, typename... Args>
	void register_function(const entt::registry & r, const char * name, const script_language_helper::function<Ret(Args...)> & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, log, "python", "Registering function %s", name);

		for (const auto & [e, comp] : r.view<data::python_state>().each())
			impl::register_function_with_state(comp, name, func);
	}
}