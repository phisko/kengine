#include "pythonHelper.hpp"

#include "data/PythonStateComponent.hpp"
#include "python/python_helper.hpp"
#include "logHelper.hpp"

namespace kengine::pythonHelper {
	namespace impl {
		template<typename Ret, typename ...Args>
		void registerEntityMember(py::class_<Entity> & entity, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
			if constexpr (std::is_reference_v<Ret>)
				entity.def(name, func, py::return_value_policy::reference);
			else
				entity.def(name, func);
		}

		template<typename Ret, typename ...Args>
		void registerFunctionWithState(PythonStateComponent::Data & state, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
			if constexpr (std::is_reference_v<Ret>)
				state.module_.def(name, func, py::return_value_policy::reference);
			else
				state.module_.def(name, func);
		}

		template<typename T>
		void registerTypeWithState(PythonStateComponent::Data & state) noexcept {
			if constexpr (std::is_same<T, Entity>()) {
				state.entity = new py::class_<Entity>(state.module_, putils::reflection::get_class_name<Entity>(), py::dynamic_attr());
				putils::reflection::for_each_attribute<Entity>([&](const auto & attr) noexcept {
					state.entity->def_readwrite(attr.name, attr.ptr);
				});
				putils::reflection::for_each_method<Entity>([&](const auto & attr) noexcept {
					state.entity->def(attr.name, attr.ptr);
				});
			}
			else
				putils::python::registerType<T>(state.module_);
		}

		template<typename T>
		void registerComponentWithState(PythonStateComponent::Data & state) noexcept {
			registerTypeWithState<T>(state);
			scriptLanguageHelper::registerComponent<T>(
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
	void registerTypes() noexcept {
		putils::for_each_type<Types...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);

			kengine_logf(Log, "Python/registerTypes", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : entities.with<PythonStateComponent>())
				impl::registerTypeWithState<T>(*comp.data);
		});
	}

	template<typename ... Comps>
	void registerComponents() noexcept {
		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using T = putils_wrapped_type(t);

			kengine_logf(Log, "Python/registerComponents", "Registering %s", putils::reflection::get_class_name<T>());
			for (const auto & [e, comp] : entities.with<PythonStateComponent>())
				impl::registerComponentWithState<T>(*comp.data);
		});
	}

	template<typename Ret, typename ...Args>
	void registerFunction(const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept {
		kengine_logf(Log, "Python/registerFunction", "Registering %s", name);
		for (const auto & [e, comp] : entities.with<PythonStateComponent>())
			impl::registerFunctionWithState(*comp.data, name, func);
	}
}
