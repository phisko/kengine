#pragma once

#include <utility>
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
        void execute(const std::function<void()> & betweenSystems = []{}) noexcept {
            if (_first) {
                _first = false;
                resetTimers();
            }

            updateSystemList();

            for (auto & [type, s] : _systems) {
                auto & time = s->time;
                auto & timer = time.timer;

				if (time.alwaysCall || timer.isDone()) {
					updateTime(*s);
					s->execute();
					for (const auto & f : _afterSystem)
						f();
					_afterSystem.clear();
					betweenSystems();
				}
			}
		}
		
    public:
		void runAfterSystem(const std::function<void()> & func) { _afterSystem.push_back(func); }

    private:
        void updateSystemList() noexcept {
            for (auto &p : _toAdd)
                _systems.emplace(p.first, std::move(p.second));
            _toAdd.clear();

            for (const auto index : _toRemove) {
                const auto it = _systems.find(index);
                if (it != _systems.end())
                    _systems.erase(it);
            }
            _toRemove.clear();
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
            time.deltaTime = (time.lastCall - old) * _speed;
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
            _toAdd.emplace_back(type, std::move(system));

            if (_first)
                updateSystemList();

            return ret;
        }

    public:
        template<typename ...Systems>
        void loadSystems(const std::string & pluginDir = "", const std::string & creatorFunction = "getSystem", bool pluginsFirst = false) {
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
        template<typename String, typename String2>
        void loadPlugins(String && pluginDir, String2 && creatorFunction) {
            auto & em = static_cast<kengine::EntityManager &>(*this);

            putils::PluginManager pm(FWD(pluginDir));
            const auto systems = pm.executeWithReturn<kengine::ISystem *>(FWD(creatorFunction), em);

            for (auto s : systems)
                addSystem(std::unique_ptr < kengine::ISystem > (s));
        }

    public:
        template<typename T>
        T & getSystem() {
            static_assert(std::is_base_of<ISystem, T>::value, "Attempt to get something that isn't a System");
            return static_cast<T &>(*_systems.at(pmeta::type<T>::index));
        }

        template<typename T>
        const T & getSystem() const {
            static_assert(std::is_base_of<ISystem, T>::value, "Attempt to get something that isn't a System");
            return static_cast<const T &>(*_systems.at(pmeta::type<T>::index));
        }

        template<typename T>
        bool hasSystem() const noexcept {
            static_assert(std::is_base_of<ISystem, T>::value, "Attempt to check something that isn't a System");
            return _systems.find(pmeta::type<T>::index) != _systems.end();
        }

        template<typename T>
        void removeSystem() noexcept {
            static_assert(std::is_base_of<ISystem, T>::value, "Attempt to remove something that isn't a System");
            _toRemove.emplace_back(pmeta::type<T>::index);
        }

    public:
        void setSpeed(float speed) noexcept { _speed = speed; }
        float getSpeed() const noexcept { return _speed; }
        void pause() noexcept { _speed = 0; }
        void resume() noexcept { _speed = 1; }

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
        float _speed = 1;
        std::vector<std::pair<pmeta::type_index, std::unique_ptr<ISystem>>> _toAdd;
        std::vector<pmeta::type_index> _toRemove;
        std::unordered_map<pmeta::type_index, std::unique_ptr<ISystem>> _systems;

		std::vector<std::function<void()>> _afterSystem;
    };
}
