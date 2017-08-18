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
#include "Timer.hpp"

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
        void execute() const
        {
            for (auto & [type, s] : _systems)
            {
                auto &time = s->time;
                auto &timer = time.timer;

                if (time.alwaysCall || timer.isDone())
                {
                    time.deltaTime = timer.getTimeSinceStart();
                    timer.setStart(
                            putils::Timer::t_clock::now() -
                                    std::chrono::duration_cast<putils::Timer::t_clock::duration>(timer.getTimeSinceDone())
                    );

                    try
                    {
                        s->execute();
                    }
                    catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
                }
            }
        }

    public:
        template<typename T, typename ...Args>
        void createSystem(Args &&...args)
        {
            static_assert(std::is_base_of<ISystem, T>::value,
                          "Attempt to create something that's not a System");
            addSystem(std::make_unique<T>(std::forward<Args>(args)...));
        }

        void addSystem(std::unique_ptr<ISystem> &&system)
        {
            const auto nbFrames = system->getFrameRate();

            auto &time = system->time;
            if (nbFrames == 0)
            {
                time.alwaysCall = true;
                time.fixedDeltaTime = std::chrono::seconds(0);
            }
            else
            {
                time.alwaysCall = false;
                time.fixedDeltaTime = std::chrono::milliseconds(1000 / nbFrames);
            }
            time.timer.setDuration(time.fixedDeltaTime);

            addModule(*system);
            const auto type = system->getType();

            _systems.emplace(type, std::move(system));
        }

    public:
        template<typename ...Systems>
        void loadSystems(std::string_view pluginDir = "plugins", std::string_view creatorFunction = "getSystem", bool pluginsFirst = false)
        {
            if (pluginsFirst)
                loadPlugins(pluginDir, creatorFunction);

            // Call "creatorFunc" in each plugin, passing myself as an EntityManager
            auto &em = static_cast<kengine::EntityManager &>(*this);

            pmeta::tuple_for_each(std::tuple<pmeta::type<Systems>...>(),
                    [this, &em](auto &&type)
                    {
                        using System = pmeta_wrapped(type);
                        createSystem<System>(em);
                    }
            );

            if (!pluginsFirst)
                loadPlugins(pluginDir, creatorFunction);
        }

    public:
        template<typename T>
        T &getSystem() { return static_cast<T &>(*_systems.at(pmeta::type<T>::index)); }

        template<typename T>
        const T &getSystem() const { return static_cast<const T &>(*_systems.at(pmeta::type<T>::index)); }

        /*
         * Internal
         */

    protected:
        void registerGameObject(GameObject &gameObject) noexcept
        {
            for (auto & [type, s] : _systems)
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
            for (auto & [type, s] : _systems)
            {
                try
                {
                    s->removeGameObject(gameObject);
                }
                catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
            }
        }

    private:
        template<typename StringView>
        void loadPlugins(StringView pluginDir, StringView creatorFunction)
        {
            putils::PluginManager pm(pluginDir);

            // Call "creatorFunc" in each plugin, passing myself as an EntityManager
            auto &em = static_cast<kengine::EntityManager &>(*this);

            const auto systems = pm.executeWithReturn<kengine::ISystem *>(
                    creatorFunction, em
            );

            for (auto s : systems)
                addSystem(std::unique_ptr<kengine::ISystem>(s));
        }

    private:
        std::unordered_map<pmeta::type_index, std::unique_ptr<ISystem>> _systems;
    };
}
