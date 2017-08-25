#pragma once

#include "System.hpp"

#include "lua/lua.hpp"
#include "reflection/Reflectible.hpp"

#include "EntityManager.hpp"
#include "common/components/LuaComponent.hpp"

namespace kengine
{
    class LuaSystem : public kengine::System<LuaSystem>
    {
    public:
        LuaSystem(kengine::EntityManager &em) : _em(em)
        {
            try { addScriptDirectory("scripts"); }
            catch (const std::runtime_error &e) {}

            _lua.open_libraries();

            _lua.set_function("getGameObjects", [&em] { return std::ref(em.getGameObjects()); });

            _lua.set_function("createEntity",
                              [&em] (const std::string &type, const std::string &name, const sol::function &f)
                              { return std::ref(em.createEntity(type, name, f)); }
            );
            _lua.set_function("removeEntity",
                              [&em] (const std::string &name)
                              { em.removeEntity(name); }
            );
            _lua.set_function("getEntity",
                              [&em] (std::string_view name)
                              { return std::ref(em.getEntity(name)); }
            );
            _lua.set_function("hasEntity",
                              [&em] (std::string_view name)
                              { return em.hasEntity(name); }
            );

            _lua.set_function("getDeltaTime", [this] { return time.getDeltaTime(); });
            _lua.set_function("getFixedDeltaTime", [this] { return time.getFixedDeltaTime(); });
            _lua.set_function("getDeltaFrames", [this] { return time.getDeltaFrames(); });

            _lua.set_function("stopRunning", [&em] { em.running = false; });

            registerType<kengine::GameObject>();
        }

        template<typename T>
        void registerType() noexcept
        {
            putils::lua::registerType<T>(_lua);

            const auto sender = putils::concat("send", T::get_class_name());
            _lua.set_function(sender, [this](const T &packet) { send(packet); });

            if constexpr (kengine::is_component<T>::value)
                registerComponent<T>();
        }

        template<typename ...Types>
        void registerTypes() noexcept
        {
            pmeta::tuple_for_each(std::make_tuple(pmeta::type<Types>()...),
                                  [this](auto &&t)
                                  {
                                      using Type = pmeta_wrapped(t);
                                      registerType<Type>();
                                  }
            );
        }

    public:
        sol::state &getState() { return _lua; }
        const sol::state &getState() const { return _lua; }

    public:
        void addScriptDirectory(std::string_view dir)
        {
            try
            {
                putils::Directory d(dir);
                _directories.push_back(dir.data());
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        // System methods
    public:
        void execute() final
        {
            executeDirectories();
            executeScriptedObjects();
        }

        // Helpers
    private:
        template<typename T>
        void registerComponent() noexcept
        {
            _lua[putils::concat("getGameObjectsWith", T::get_class_name())] =
                    [this] { return std::ref(_em.getGameObjects<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("get", T::get_class_name())] =
                    [](kengine::GameObject &self) { return std::ref(self.getComponent<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("has", T::get_class_name())] =
                    [](kengine::GameObject &self) { return self.hasComponent<T>(); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("attach", T::get_class_name())] =
                    [](kengine::GameObject &self) { return std::ref(self.attachComponent<T>()); };

            _lua[kengine::GameObject::get_class_name()][putils::concat("detach", T::get_class_name())] =
                    [](kengine::GameObject &self) { self.detachComponent<T>(); };
        }

        template<typename F>
        void addEntityFunction(const std::string &name, F &&func) noexcept
        {
            _lua[kengine::GameObject::get_class_name()][name] = func;
        }

        void executeDirectories() noexcept
        {
            for (const auto &dir : _directories)
            {
                try
                {
                    putils::Directory d(dir);

                    d.for_each(
                            [this](const putils::Directory::File &f)
                            {
                                if (!f.isDirectory)
                                    executeScript(f.fullPath);
                            }
                    );
                }
                catch (const std::runtime_error &e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        }

        void executeScriptedObjects() noexcept
        {
            struct Create
            {
                std::string type;
                std::string name;
                std::function<void(kengine::GameObject &)> postCreate;
            };

            std::vector<std::function<void()>> toExecute;
            _lua["createEntity"] =
                    [this, &toExecute] (const std::string &type, const std::string &name, const sol::function &f)
                    {
                        toExecute.push_back([this, type, name, f]{ _em.createEntity(type, name, f); });
                    };

            _lua["removeEntity"] =
                    [this, &toExecute](const std::string &name)
                    {
                        toExecute.push_back([this, name]{ _em.removeEntity(name); });
                    };

            for (const auto go : _em.getGameObjects<kengine::LuaComponent>())
            {
                const auto &comp = go->getComponent<kengine::LuaComponent>();
                for (const auto &s : comp.getScripts())
                {
                    _lua["self"] = go;
                    executeScript(s);
                    _lua["self"] = sol::nil;
                }
            }

            for (const auto &f : toExecute)
                f();

            _lua["createEntity"] =
                    [this] (const std::string &type, const std::string &name, const sol::function &f)
                    { return std::ref(_em.createEntity(type, name, f)); };
            _lua["removeEntity"] =
                    [this] (const std::string &name)
                    { _em.removeEntity(name); };
        }

        void executeScript(std::string_view fileName) noexcept
        {
            try
            {
                _lua.script_file(fileName.data());
            }
            catch (const std::exception &e)
            {
                std::cerr << "[LuaSystem] Error in '" << fileName << "': " << e.what() << std::endl;
            }
        }

    private:
        kengine::EntityManager &_em;
        std::vector<std::string> _directories;
        sol::state _lua;
    };
}