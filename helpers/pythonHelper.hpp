#pragma once

#include "ScriptLanguageHelper.hpp"
#include "data/PythonStateComponent.hpp"
#include "python.hpp"

namespace kengine::pythonHelper {
	template<typename T>
	void registerType(EntityManager & em);

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
				state.module.def(name, func, py::return_value_policy::reference);
			else
				state.module.def(name, func);
		}

		template<typename T>
		void registerTypeWithState(EntityManager & em, PythonStateComponent::Data & state) {
			if constexpr (std::is_same<T, Entity>()) {
				state.entity = new py::class_<Entity>(state.module, putils::reflection::get_class_name<Entity>(), py::dynamic_attr());
				putils::reflection::for_each_attribute<Entity>([&](auto name, auto member) {
					state.entity->def_readwrite(name, member);
				});
				putils::reflection::for_each_method<Entity>([&](auto name, auto member) {
					state.entity->def(name, member);
				});
			}
			else
				putils::python::registerType<T>(state.module);

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

	template<typename Ret, typename ...Args>
	void registerFunction(EntityManager & em, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) {
		for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
			detail::registerFunctionWithState(*comp.data, name, func);
	}
}