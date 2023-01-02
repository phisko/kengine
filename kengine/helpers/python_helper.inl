#include "python_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/python/python_helper.hpp"

// kengine data
#include "kengine/data/python_state.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

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

		template<typename T>
		void register_type_with_state(data::python_state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

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
		}

		template<typename T>
		void register_component_with_state(entt::registry & r, data::python_state & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			register_type_with_state<T>(state);
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

	template<typename... Types>
	void register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Types...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);

			kengine_logf(r, log, "python/register_types", "Registering %s", putils::reflection::get_class_name<type>());
			for (const auto & [e, comp] : r.view<data::python_state>().each())
				impl::register_type_with_state<type>(comp);
		});
	}

	template<typename... Comps>
	void register_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);

			kengine_logf(r, log, "python/register_components", "Registering %s", putils::reflection::get_class_name<type>());
			for (const auto & [e, comp] : r.view<data::python_state>().each())
				impl::register_component_with_state<type>(r, comp);
		});
	}

	template<typename Ret, typename... Args>
	void register_function(const entt::registry & r, const char * name, const script_language_helper::function<Ret(Args...)> & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, log, "python/register_function", "Registering %s", name);

		for (const auto & [e, comp] : r.view<data::python_state>().each())
			impl::register_function_with_state(comp, name, func);
	}
}