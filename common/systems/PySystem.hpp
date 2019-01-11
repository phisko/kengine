#pragma once

#include "ScriptSystem.hpp"
#include "python.hpp"
#include "components/PyComponent.hpp"

namespace kengine {
    class PySystem : public kengine::ScriptSystem<PySystem, PyComponent> {
    public:
        PySystem(EntityManager & em) : ScriptSystem(em) {
            py::globals()["pk"] = _m;
			ScriptSystem::init();
			_m.attr("self") = &_self;
        }

    public:
		template<typename Ret, typename ...Args>
		void registerFunction(const std::string & name, const std::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				_m.def(name.c_str(), func, py::return_value_policy::reference);
			else
				_m.def(name.c_str(), func);
		}

		template<typename Ret, typename ...Args>
		void registerEntityMember(const std::string & name, const std::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				_go->def(name.c_str(), func, py::return_value_policy::reference);
			else
				_go->def(name.c_str(), func);
		}

		template<typename T>
		void registerTypeInternal() {
			if constexpr (std::is_same<T, EntityView>::value || std::is_same<T, Entity>::value) {
				_go = new py::class_<EntityView>(_m, EntityView::get_class_name(), py::dynamic_attr());
				putils::for_each_attribute(EntityView::get_attributes(), [this](auto name, auto member) {
					_go->def_readwrite(name, member);
				});
				putils::for_each_attribute(EntityView::get_methods(), [this](auto name, auto member) {
					_go->def(name, member);
				});
				// _go->def("__str__", [](EntityView obj) { return putils::to_string(obj); });
			} else
				putils::python::registerType<T>(_m);
		}

    public:
		void executeScript(const char * fileName) noexcept {
			py::eval_file(fileName, py::globals());
		}

	public:
		void setSelf(EntityView go) { _self = go;  }
		void unsetSelf() const {}

	private:
		py::scoped_interpreter _guard;
		py::module _m{ "pk" };
		py::class_<EntityView> * _go;
		EntityView _self { Entity::INVALID_ID, 0 };
	};
}
