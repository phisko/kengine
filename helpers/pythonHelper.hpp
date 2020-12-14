#pragma once

#include "ScriptLanguageHelper.hpp"
#include "data/PythonStateComponent.hpp"
#include "python/python_helper.hpp"

namespace kengine::pythonHelper {
	template<typename T>
	void registerType(EntityManager & em);
	template<typename ... Types>
	void registerTypes(EntityManager & em);

	template<typename Comp>
	void registerComponent(EntityManager & em);
	template<typename ... Comps>
	void registerComponents(EntityManager & em);

	template<typename Ret, typename ...Args>
	void registerFunction(EntityManager & em, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func);
}

// Impl
namespace kengine::pythonHelper {
	namespace detail {
		template<typename Ret, typename ...Args>
		void registerEntityMember(py::class_<Entity> & entity, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				entity.def(name, func, py::return_value_policy::reference);
			else
				entity.def(name, func);
		}

		template<typename Ret, typename ...Args>
		void registerFunctionWithState(PythonStateComponent::Data & state, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				state.module_.def(name, func, py::return_value_policy::reference);
			else
				state.module_.def(name, func);
		}

		template<typename T>
		void registerTypeWithState(EntityManager & em, PythonStateComponent::Data & state) {
			if constexpr (std::is_same<T, Entity>()) {
				state.entity = new py::class_<Entity>(state.module_, putils::reflection::get_class_name<Entity>(), py::dynamic_attr());
				putils::reflection::for_each_attribute<Entity>([&](auto name, auto member) {
					state.entity->def_readwrite(name, member);
				});
				putils::reflection::for_each_method<Entity>([&](auto name, auto member) {
					state.entity->def(name, member);
				});
			}
			else
				putils::python::registerType<T>(state.module_);
		}

		template<typename T>
		void registerComponentWithState(EntityManager & em, PythonStateComponent::Data & state) {
			registerTypeWithState<T>(em, state);
			scriptLanguageHelper::registerComponent<T>(
				em,
				[&](auto && ... args) {
					registerEntityMember(*state.entity, FWD(args)...);
				},
				[&](auto && ... args) {
					registerFunctionWithState(state, FWD(args)...);
				}
			);
		}
	}

	template<typename T>
	void registerType(EntityManager & em) {
		for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
			detail::registerTypeWithState<T>(em, *comp.data);
	}

	template<typename ...Types>
	void registerTypes(EntityManager & em) {
		putils::for_each_type<Types...>([&](auto && t) {
			using T = putils_wrapped_type(t);
			registerType<T>(em);
		});
	}

	template<typename Comp>
	void registerComponent(EntityManager & em) {
		for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
			detail::registerComponentWithState<Comp>(em, *comp.data);
	}

	template<typename ... Comps>
	void registerComponents(EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto && t) {
			using T = putils_wrapped_type(t);
			registerComponent<T>(em);
		});
	}

	template<typename Ret, typename ...Args>
	void registerFunction(EntityManager & em, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) {
		for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
			detail::registerFunctionWithState(*comp.data, name, func);
	}
}
