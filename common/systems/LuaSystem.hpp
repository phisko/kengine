#pragma once

#include "System.hpp"

#include "lua/lua.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine
{
    class LuaSystem : public kengine::System<LuaSystem, kengine::AllComponents>
    {
    public:
        LuaSystem(kengine::EntityManager &em)
        {
            _lua.open_libraries(sol::lib::base, sol::lib::base);

            _lua.set_function("getGameObjects", [this] { return getGameObjects(); });
            _lua.set_function("createEntity",
                              [&em] (const std::string &type, const std::string &name)
                              { return std::ref(em.createEntity(type, name)); }
            );
            _lua.set_function("removeEntity",
                              [&em] (kengine::GameObject &go)
                              { em.removeEntity(go); }
            );
            _lua.set_function("getEntity",
                              [&em] (std::string_view name)
                              { return std::ref(em.getEntity(name)); }
            );

            registerType<kengine::GameObject>();
        }

        template<typename T>
        void registerType()
        {
            putils::lua::registerType<T>(_lua);

            if constexpr (kengine::is_component<T>::value)
            {
                const auto getter = putils::concat("get", T::get_class_name());
                _lua[kengine::GameObject::get_class_name()][getter] =
                    [](kengine::GameObject &self)
                    {
                        return std::ref(self.getComponent<T>());
                    };

                const auto checker = putils::concat("has", T::get_class_name());
                _lua[kengine::GameObject::get_class_name()][checker] =
                    [](kengine::GameObject &self)
                    {
                        return self.hasComponent<T>();
                    };
            }
        }

        // System methods
        void execute() final
        {
            putils::Directory d("scripts");

            d.for_each(
                    [this](const putils::Directory::File &f)
                    {
                        try
                        {
                            _lua.script_file(f.fullPath);
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "[LuaSystem] Error in '" << f.fullPath << "': " << e.what() << std::endl;
                        }
                    }
            );
        }

    private:
        sol::state _lua;
    };
}