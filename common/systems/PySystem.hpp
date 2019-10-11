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
			if constexpr (std::is_same<T, Entity>::value) {
				_go = new py::class_<Entity>(_m, Entity::get_class_name(), py::dynamic_attr());
				putils::for_each_attribute(Entity::get_attributes(), [this](auto name, auto member) {
					_go->def_readwrite(name, member);
				});
				putils::for_each_attribute(Entity::get_methods(), [this](auto name, auto member) {
					_go->def(name, member);
				});
			} else
				putils::python::registerType<T>(_m);
		}

    public:
		void executeScript(const char * fileName) noexcept {
			try {
				py::eval_file(fileName, py::globals());
			}
			catch (const std::exception & e) {
				std::cerr << putils::termcolor::red << e.what() << '\n' << putils::termcolor::reset;
			}
		}

	public:
		void setSelf(Entity & go) { _m.attr("self") = &go; }
		void unsetSelf() const { _m.attr("self") = nullptr; }

	private:
		py::scoped_interpreter _guard;
		py::module _m{ "pk" };
		py::class_<Entity> * _go;
	};
}
