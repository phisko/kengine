#include "pythonHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// putils
#include "python/python_helper.hpp"

// kengine data
#include "data/PythonStateComponent.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::pythonHelper {
	namespace impl {
		template<typename Ret, typename ...Args>
		void registerEntityMember(py::class_<entt::handle> & entity, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				entity.def(name, func, py::return_value_policy::reference);
			else
				entity.def(name, func);
		}

		template<typename Ret, typename ...Args>
		void registerFunctionWithState(PythonStateComponent::Data & state, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				state.module_.def(name, func, py::return_value_policy::reference);
			else
				state.module_.def(name, func);
		}

		template<typename T>
		void registerTypeWithState(PythonStateComponent::Data & state) noexcept {
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
				putils::python::registerType<T>(state.module_);
		}

		template<typename T>
		void registerComponentWithState(entt::registry & r, PythonStateComponent::Data & state) noexcept {
			KENGINE_PROFILING_SCOPE;

			registerTypeWithState<T>(state);
			scriptLanguageHelper::registerComponent<T>(
				r,
				[&](auto && ... args) noexcept {
					registerEntityMember(*state.entity, FWD(args)...);
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

			kengine_logf(r, Log, "Python/registerTypes", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : r.view<PythonStateComponent>().each())
				impl::registerTypeWithState<T>(*comp.data);
		});
	}

	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using T = putils_wrapped_type(t);

			kengine_logf(r, Log, "Python/registerComponents", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : r.view<PythonStateComponent>().each())
				impl::registerComponentWithState<T>(r, *comp.data);
		});
	}

	template<typename Ret, typename ...Args>
	void registerFunction(const entt::registry & r, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, Log, "Python/registerFunction", "Registering %s", name);

		for (const auto & [e, comp] : r.view<PythonStateComponent>().each())
			impl::registerFunctionWithState(*comp.data, name, func);
	}
}