#pragma once

#pragma once

#include "ScriptSystem.hpp"
#include "data/LuaStateComponent.hpp"
#include "data/PythonStateComponent.hpp"
#include "python.hpp"

#include "EntityCreator.hpp"

namespace kengine {
	EntityCreatorFunctor<64> PySystem(EntityManager & em);

	namespace python {
		template<typename Ret, typename ...Args>
		void registerEntityMember(py::class_<Entity> & entity, const std::string & name, const std::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				entity.def(name.c_str(), func, py::return_value_policy::reference);
			else
				entity.def(name.c_str(), func);
		}

		template<typename T>
		void registerTypeWithState(PythonStateComponent::Data & state) {
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

			ScriptSystem::registerComponent<T>([&](auto && ... args) {
				registerEntityMember(*state.entity, FWD(args)...);
				});
		}

		template<typename T>
		void registerType(EntityManager & em) {
			for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
				registerTypeWithState<T>(*comp.data);
		}

		template<typename Ret, typename ...Args>
		void registerFunctionWithState(PythonStateComponent::Data & state, const std::string & name, const std::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				state.module.def(name.c_str(), func, py::return_value_policy::reference);
			else
				state.module.def(name.c_str(), func);
		}

		template<typename Ret, typename ...Args>
		void registerFunction(EntityManager & em, const std::string & name, const std::function<Ret(Args...)> & func) {
			for (const auto & [e, comp] : em.getEntities<PythonStateComponent>())
				registerFunctionWithState(*comp.data);
		}
	}
}
