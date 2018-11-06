#pragma once

#include "ScriptSystem.hpp"
#include "Directory.hpp"
#include "python.hpp"
#include "components/PyComponent.hpp"

namespace kengine {
    class PySystem : public kengine::ScriptSystem<PySystem, PyComponent> {
    public:
        PySystem(kengine::EntityManager & em) : ScriptSystem(em) {
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
		void registerGameObjectMember(const std::string & name, const std::function<Ret(Args...)> & func) {
			if constexpr (std::is_reference_v<Ret>)
				_go->def(name.c_str(), func, py::return_value_policy::reference);
			else
				_go->def(name.c_str(), func);
		}

		template<typename T>
		void registerTypeInternal() {
			if constexpr (std::is_same<T, kengine::GameObject>::value) {
				_go = new py::class_<GameObject>(_m, GameObject::get_class_name(), py::dynamic_attr());
				pmeta::tuple_for_each(GameObject::get_attributes().getKeyValues(), [this](auto && p) {
					_go->def_readwrite(p.first.data(), p.second);
				});
				pmeta::tuple_for_each(GameObject::get_methods().getKeyValues(), [this](auto && p) {
					_go->def(p.first.data(), p.second);
				});
				_go->def("__str__", [](const GameObject & obj) { return putils::to_string(obj); });
			} else
				putils::python::registerType<T>(_m);
		}

    public:
		void executeScript(const std::string & fileName) noexcept {
			try {
				py::eval_file(fileName, py::globals());
			}
			catch (const std::exception & e) {
				std::cerr << "[PySystem] Error in '" << fileName << "': " << e.what() << std::endl;
			}
		}

	public:
		void setSelf(kengine::GameObject & go) { _m.attr("self") = &go; }
		void unsetSelf() { _m.attr("self") = nullptr; }

	private:
		py::scoped_interpreter _guard;
		py::module _m{ "pk" };
		py::class_<GameObject> * _go;
	};
}