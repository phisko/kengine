#pragma once

#include <cmath>
#include <vector>
#include <memory>
#include "System.hpp"
#include "GameObject.hpp"
#include "Mediator.hpp"
#include "pluginManager/PluginManager.hpp"
#include "Timer.hpp"
#include "common/packets/RegisterGameObject.hpp"
#include "common/packets/RemoveGameObject.hpp"

namespace kengine {
    class EntityManager;

    class SystemManager : public putils::Mediator {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

    public:
        SystemManager(SystemManager const & o) = delete;
        SystemManager & operator=(SystemManager const & o) = delete;

    public:
        void execute() {
            if (_first) {
                _first = false;
                resetTimers();
            }

            for (auto & [type, s] : _systems) {
                auto & time = s->time;
                auto & timer = time.timer;

                if (time.alwaysCall || timer.isDone()) {
                    updateTime(*s);
                    try { s->execute(); }
                    catch (const std::exception & e) { std::cerr << e.what() << std::endl; }
                }
            }
        }

    private:
        void resetTimers() {
            for (auto & [type, s] : _systems) {
                s->time.lastCall = putils::Timer::t_clock::now();
                s->time.timer.restart();
            }
        }

    private:
        void updateTime(kengine::ISystem & s) {
            auto & time = s.time;
            auto & timer = time.timer;

            const auto old = time.lastCall;
            time.lastCall = putils::Timer::t_clock::now();
            time.deltaTime = time.lastCall - old;
            const auto past = std::fmod(timer.getTimeSinceDone().count(), timer.getDuration().count());
            const auto dur = std::chrono::duration_cast<putils::Timer::t_clock::duration>(putils::Timer::seconds(past));
            timer.setStart(time.lastCall - dur);
        }

    private:
        bool _first = true;

    public:
        template<typename T, typename ...Args>
        T & createSystem(Args && ...args) {
            static_assert(std::is_base_of<ISystem, T>::value,
                          "Attempt to create something that's not a System");
            return static_cast<T &>(addSystem(std::make_unique<T>(FWD(args)...)));
        }

        ISystem & addSystem(std::unique_ptr<ISystem> && system) {
            const auto nbFrames = system->getFrameRate();

            auto & time = system->time;
            if (nbFrames == 0) {
                time.alwaysCall = true;
                time.fixedDeltaTime = std::chrono::seconds(0);
            } else {
                time.alwaysCall = false;
                time.fixedDeltaTime = std::chrono::milliseconds(1000 / nbFrames);
            }
            time.lastCall = putils::Timer::t_clock::now();
            time.timer.setDuration(time.fixedDeltaTime);

            addModule(*system);
            const auto type = system->getType();

            auto & ret = *system;
            _systems.emplace(type, std::move(system));
            return ret;
        }

    public:
        template<typename ...Systems>
        void loadSystems(std::string_view pluginDir = "", std::string_view creatorFunction = "getSystem", bool pluginsFirst = false) {
            if (pluginsFirst && pluginDir.size() > 0)
                loadPlugins(pluginDir, creatorFunction);

            // Call "creatorFunc" in each plugin, passing myself as an EntityManager
            auto & em = static_cast<kengine::EntityManager &>(*this);

            pmeta::tuple_for_each(std::tuple < pmeta::type<Systems>... > (),
                    [this, &em](auto && type) {
                        using System = pmeta_wrapped(type);
                        createSystem<System>(em);
                    }
            );

            if (!pluginsFirst && pluginDir.size() > 0)
                loadPlugins(pluginDir, creatorFunction);
        }

    private:
        template<typename StringView>
        void loadPlugins(StringView pluginDir, StringView creatorFunction) {
            auto & em = static_cast<kengine::EntityManager &>(*this);

            putils::PluginManager pm(pluginDir);
            const auto systems = pm.executeWithReturn<kengine::ISystem *>(creatorFunction, em);

            for (auto s : systems)
                addSystem(std::unique_ptr < kengine::ISystem > (s));
        }

    public:
        template<typename T>
        T & getSystem() { return static_cast<T &>(*_systems.at(pmeta::type<T>::index)); }

        template<typename T>
        const T & getSystem() const { return static_cast<const T &>(*_systems.at(pmeta::type<T>::index)); }

        /*
         * Internal
         */

    protected:
        void registerGameObject(GameObject & gameObject) noexcept {
            send(kengine::packets::RegisterGameObject{ gameObject });
        }

        void removeGameObject(GameObject & gameObject) {
            send(kengine::packets::RemoveGameObject{ gameObject });
        }

    private:
        std::unordered_map<pmeta::type_index, std::unique_ptr<ISystem>> _systems;
    };
}
