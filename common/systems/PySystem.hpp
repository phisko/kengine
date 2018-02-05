#pragma once

#include "System.hpp"
#include "Directory.hpp"
#include "python.hpp"
#include "components/PyComponent.hpp"

namespace kengine {
    class PySystem : public kengine::System<PySystem> {
    public:
        PySystem(kengine::EntityManager & em) : _em(em) {
            addScriptDirectory("python");

            locals["pk"] = _m;

            _m.def("getGameObjects",
                   [&em]() -> const std::vector<kengine::GameObject *> & { return em.getGameObjects(); },
                   py::return_value_policy::reference
            );

            _m.def("createEntity",
                   [&em](const std::string & type, const std::string & name, const py::function & postCreate) -> kengine::GameObject & {
                       return em.createEntity(type, name, [&postCreate](kengine::GameObject & go) {
                           return postCreate.operator()<py::return_value_policy::reference>(go);
                       });
                   }, py::return_value_policy::reference
            );
            _m.def("createNamelessEntity",
                   [&em](const std::string & type, const py::function & postCreate) -> kengine::GameObject & {
                       return em.createEntity(type, [&postCreate](kengine::GameObject & go) {
                           return postCreate.operator()<py::return_value_policy::reference>(go);
                       });
                   }, py::return_value_policy::reference
            );

            _m.def("removeEntity", [&em](kengine::GameObject & go) { em.removeEntity(go); });
            _m.def("removeEntityByName", [&em](const std::string & name) { em.removeEntity(name); });

            _m.def("getEntity",
                   [&em](const std::string & name) -> kengine::GameObject & { return em.getEntity(name); },
                   py::return_value_policy::reference
            );
            _m.def("hasEntity", [&em](const std::string & name) { return em.hasEntity(name); });

            _m.def("getDeltaTime", [this] { return time.getDeltaTime(); });
            _m.def("getFixedDeltaTime", [this] { return time.getFixedDeltaTime(); });
            _m.def("getDeltaFrames", [this] { return time.getDeltaFrames(); });

            _m.def("stopRunning", [&em] { em.running = false; });

            _go = new py::class_<GameObject>(_m, GameObject::get_class_name(), py::dynamic_attr());
            pmeta::tuple_for_each(GameObject::get_attributes().getKeyValues(), [this](auto && p) {
                _go->def_readwrite(p.first.data(), p.second);
            });
            pmeta::tuple_for_each(GameObject::get_methods().getKeyValues(), [this](auto && p) {
                _go->def(p.first.data(), p.second);
            });
            _go->def("__str__", [](const GameObject & obj) { return putils::to_string(obj); });
        }

    public:
        template<typename T>
        void registerType() noexcept {
            putils::python::registerType<T>(_m);

            const auto sender = putils::concat("send", T::get_class_name());
            _m.def(sender.c_str(), [this](const T & packet) { send(packet); });

            if constexpr (kengine::is_component<T>::value)
                registerComponent<T>();
        }

        template<typename ...Types>
        void registerTypes() noexcept {
            pmeta::tuple_for_each(std::make_tuple(pmeta::type<Types>()...),
                                  [this](auto && t) {
                                      using Type = pmeta_wrapped(t);
                                      registerType<Type>();
                                  }
            );
        }

    private:
        template<typename T>
        void registerComponent() {
            _m.def(putils::concat("getGameObjectsWith", T::get_class_name()).c_str(),
                   [this]() -> const std::vector<kengine::GameObject *> & { return _em.getGameObjects<T>(); },
                   py::return_value_policy::reference
            );

            _go->def(putils::concat("get", T::get_class_name()).c_str(),
                     [](kengine::GameObject & self) -> T & { return self.getComponent<T>(); },
                     py::return_value_policy::reference
            );

            _go->def(putils::concat("has", T::get_class_name()).c_str(),
                     [](kengine::GameObject & self) { return self.hasComponent<T>(); }
            );

            _go->def(putils::concat("attach", T::get_class_name()).c_str(),
                     [](kengine::GameObject & self) -> T & { return self.attachComponent<T>(); },
                     py::return_value_policy::reference
            );

            _go->def(putils::concat("detach", T::get_class_name()).c_str(),
                     [](kengine::GameObject & self) { self.detachComponent<T>(); }
            );
        }

    public:
        template<typename String>
        void addScriptDirectory(String && dir) noexcept {
            try {
                putils::Directory d(dir);
                _directories.emplace_back(FWD(dir));
            } catch (const std::runtime_error & e) {
                std::cerr << e.what() << std::endl;
            }
        }

    public:
        void execute() noexcept final {
            executeDirectories();
            executeScriptedObjects();
        }

    private:
        void executeDirectories() noexcept {
            for (const auto & dir : _directories) {
                try {
                    putils::Directory d(dir);

                    d.for_each([this](const putils::Directory::File & f) {
                                   if (!f.isDirectory)
                                       executeScript(f.fullPath);
                               }
                    );
                } catch (const std::runtime_error & e) {
                    std::cerr << e.what() << std::endl;
                }
            }
        }

        void executeScriptedObjects() noexcept {
            for (const auto go : _em.getGameObjects<PyComponent>()) {
                const auto & comp = go->getComponent<PyComponent>();
                _m.attr("self") = go;
                for (const auto & s: comp.getScripts())
                    executeScript(s);
            }
            _m.attr("self") = nullptr;
        }

        void executeScript(const std::string & fileName) noexcept {
            try {
                py::eval_file(fileName, py::globals(), locals);
            } catch (const std::exception & e) {
                std::cerr << "[PySystem] Error in '" << fileName << "': " << e.what() << std::endl;
            }
        }

    private:
        kengine::EntityManager & _em;
        std::vector<std::string> _directories;

    private:
        py::scoped_interpreter _guard;
        py::module _m{ "pk" };
        py::dict locals;
        py::class_<GameObject> * _go;
    };
}
