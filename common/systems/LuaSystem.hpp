#pragma once

#include "System.hpp"

#include "lua/plua.hpp"
#include "reflection/Reflectible.hpp"

#include "EntityManager.hpp"
#include "common/components/LuaComponent.hpp"
#include "common/packets/LuaState.hpp"

namespace kengine {
    class LuaSystem : public kengine::System<LuaSystem, kengine::packets::LuaState::Query> {
    public:
        LuaSystem(kengine::EntityManager & em) : _em(em) {
            try { addScriptDirectory("scripts"); }
            catch (const std::runtime_error & e) {}

            _lua.open_libraries();

            _lua["getGameObjects"] = [&em] { return std::ref(em.getGameObjects()); };

            _lua["createEntity"] =
                    [this](const std::string & type, const std::string & name, const sol::function & f) {
                        _toExecute.push_back([this, type, name, f] { _em.createEntity(type, name, f); });
                    };

            _lua["createNoNameEntity"] =
                    [this](const std::string & type, const sol::function & f) {
                        _toExecute.push_back([this, type, f] { _em.createEntity(type, f); });
                    };

            _lua["removeEntity"] =
                    [this](const std::string & name) {
                        _toExecute.push_back([this, name] { _em.removeEntity(name); });
                    };

            _lua["getEntity"] = [&em](const std::string & name) { return std::ref(em.getEntity(name)); };
            _lua["hasEntity"] = [&em](const std::string & name) { return em.hasEntity(name); };

            _lua["getDeltaTime"] = [this] { return time.getDeltaTime(); };
            _lua["getFixedDeltaTime"] = [this] { return time.getFixedDeltaTime(); };
            _lua["getDeltaFrames"] = [this] { return time.getDeltaFrames(); };

            _lua["stopRunning"] = [&em] { em.running = false; };

            registerType<kengine::GameObject>();
        }

    public:
        template<typename T>
        void registerType() noexcept {
            putils::lua::registerType<T>(_lua);

            const auto sender = putils::concat("send", T::get_class_name());
            _lua.set_function(sender, [this](const T & packet) { send(packet); });

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

    public:
        template<typename String>
        void addScriptDirectory(String && dir) {
            try {
                putils::Directory d(dir);
                _directories.emplace_back(FWD(dir));
            }
            catch (const std::runtime_error & e) {
                std::cerr << e.what() << std::endl;
            }
        }

    public:
        void handle(const kengine::packets::LuaState::Query & q) noexcept {
            sendTo(kengine::packets::LuaState::Response{ &_lua }, *q.sender);
        }

        // System methods
    public:
        void execute() final {
            executeDirectories();
            executeScriptedObjects();
            for (const auto & f: _toExecute) {
                try {
                    f();
                } catch (const std::exception &e) {
                    std::cerr << "[LuaSystem] Error: '" <<  e.what() << std::endl;
                }
            }
            _toExecute.clear();
        }

        // Helpers
    private:
        template<typename T>
        void registerComponent() noexcept {
            _lua[putils::concat("getGameObjectsWith", T::get_class_name())] =
                    [this] { return std::ref(_em.getGameObjects<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("get", T::get_class_name())] =
                    [](kengine::GameObject & self) { return std::ref(self.getComponent<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("has", T::get_class_name())] =
                    [](kengine::GameObject & self) { return self.hasComponent<T>(); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("attach", T::get_class_name())] =
                    [](kengine::GameObject & self) { return std::ref(self.attachComponent<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("detach", T::get_class_name())] =
                    [](kengine::GameObject & self) { self.detachComponent<T>(); };
        }

        template<typename F>
        void addEntityFunction(const std::string & name, F && func) noexcept {
            _lua[kengine::GameObject::get_class_name()][name] = func;
        }

        void executeDirectories() noexcept {
            for (const auto & dir : _directories) {
                try {
                    putils::Directory d(dir);

                    d.for_each(
                            [this](const putils::Directory::File & f) {
                                if (!f.isDirectory)
                                    executeScript(f.fullPath);
                            }
                    );
                }
                catch (const std::runtime_error & e) {
                    std::cerr << e.what() << std::endl;
                }
            }
        }

        void executeScriptedObjects() noexcept {
            for (const auto go : _em.getGameObjects<kengine::LuaComponent>()) {
                const auto & comp = go->getComponent<kengine::LuaComponent>();
                for (const auto & s : comp.getScripts()) {
                    _lua["self"] = go;
                    executeScript(s);
                }
            }
            _lua["self"] = sol::nil;
        }

        void executeScript(const std::string & fileName) noexcept {
            try {
                _lua.script_file(fileName);
            }
            catch (const std::exception & e) {
                std::cerr << "[LuaSystem] Error in '" << fileName << "': " << e.what() << std::endl;
            }
        }

    private:
        kengine::EntityManager & _em;
        std::vector<std::string> _directories;
        sol::state _lua;
        std::vector<std::function<void()>> _toExecute;
    };
}