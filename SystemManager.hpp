//
// Created by naliwe on 2/12/17.
//

#pragma once

#include <vector>
#include <memory>
#include "System.hpp"
#include "GameObject.hpp"
#include "Mediator.hpp"
#include "pluginManager/PluginManager.hpp"

namespace kengine
{
    class EntityManager;

    class SystemManager : public putils::Mediator
    {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

    public:
        SystemManager(SystemManager const &o) = delete;
        SystemManager &operator=(SystemManager const &o) = delete;

    public:
        void execute()
        {
            for (auto & [type, systems] : _systems)
                for (auto &s : systems)
                    s->execute();
        }

    public:
        template<typename T, typename ...Args>
        void createSystem(Args &&...args)
        {
            if constexpr (!std::is_base_of<ISystem, T>::value)
                static_assert("Attempt to create something that's not a System");

            addSystem(std::make_unique<T>(std::forward<Args>(args)...));
        }

        void addSystem(std::unique_ptr<ISystem> &&system)
        {
            addModule(system.get());
            auto &category = _systems[system->getCompType()];
            _systemsByType[system->getType()] = system.get();
            category.emplace_back(std::move(system));
        }

    public:
        template<typename ...Systems>
        void loadSystems(std::string_view pluginDir = "plugins", std::string_view creatorFunction = "getSystem")
        {
            putils::PluginManager pm(pluginDir);

            // Call "creatorFunc" in each plugin, passing myself as an EntityManager
            auto &em = static_cast<kengine::EntityManager &>(*this);

            const auto systems = pm.executeWithReturn<kengine::ISystem *>(
                    creatorFunction, em
            );

            for (auto s : systems)
                addSystem(std::unique_ptr<kengine::ISystem>(s));

            pmeta::tuple_for_each(std::tuple<pmeta::type<Systems>...>(),
                    [this, &em](auto &&type)
                    {
                        using System = pmeta_wrapped(type);
                        createSystem<System>(em);
                    }
            );
        }

    public:
        template<typename T>
        T &getSystem() { return static_cast<T &>(*_systemsByType.at(pmeta::type<T>::index)); }

        template<typename T>
        const T &getSystem() const { return static_cast<const T &>(*_systemsByType.at(pmeta::type<T>::index)); }

        /*
         * Internal
         */

    protected:
        void registerGameObject(GameObject &gameObject) noexcept
        {
            for (auto & [type, systems] : _systems)
                if (type == pmeta::type<kengine::AllComponents>::index || matchMasks(type, gameObject))
                    for (auto &s : systems)
                    {
                        try
                        {
                            s->registerGameObject(gameObject);
                        }
                        catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
                    }
        }

        void removeGameObject(GameObject &gameObject)
        {
            for (auto & [type, systems] : _systems)
                if (matchMasks(type, gameObject))
                    for (auto &s : systems)
                    {
                        try
                        {
                            s->removeGameObject(gameObject);
                        }
                        catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
                    }
        }

        // Implementation detail
    private:
        bool matchMasks(pmeta::type_index mask, const kengine::GameObject &go)
        {
            const auto &goMask = go.getTypes();
            return std::find(goMask.begin(), goMask.end(), mask) != goMask.end();
        }

    private:
        using Category = std::vector<std::unique_ptr<ISystem>>;
        std::unordered_map<pmeta::type_index, Category> _systems;

        std::unordered_map<pmeta::type_index, ISystem*> _systemsByType;
    };
}
