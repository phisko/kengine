#pragma once

#ifndef KENGINE_AFTER_SYSTEM_FUNCTION_SIZE
# define KENGINE_AFTER_SYSTEM_FUNCTION_SIZE 128
#endif

#ifndef KENGINE_MAX_SYSTEMS_LOADED_PER_FRAME
# define KENGINE_MAX_PLUGINS_LOADED_PER_FRAME 64
#endif

#include <utility>
#include <cmath>
#include <vector>
#include <memory>
#include "Mediator.hpp"
#include "pluginManager/PluginManager.hpp"
#include "Timer.hpp"
#include "common/packets/RegisterEntity.hpp"
#include "common/packets/RemoveEntity.hpp"
#include "ISystem.hpp"
#include "Component.hpp"

namespace kengine {
	class Entity;

    class SystemManager : public putils::Mediator {
    public:
		SystemManager(size_t threads = 0) : Mediator(threads) {}

    public:
        void execute() noexcept {
            if (_first) {
                _first = false;
                resetTimers();
            }

			_inFrame = true;

            updateSystemList();

            for (auto & system : _systems) {
                auto & time = system.ptr->time;
                auto & timer = time.timer;

				if (time.alwaysCall || timer.isDone()) {
					updateTime(*system.ptr);
					system.ptr->execute();
					for (const auto & f : _afterSystem)
						f();
					_afterSystem.clear();
				}
			}

			_inFrame = false;
		}
		
    public:
		template<typename Func>
		void runAfterSystem(Func && func) { _afterSystem.push_back(FWD(func)); }

	private:
		void updateSystemList() noexcept {
			for (auto & p : _toAdd)
				_systems.push_back({ p.first, std::move(p.second) });
			_toAdd.clear();

			for (const auto index : _toRemove) {
				const auto it = std::find_if(_systems.begin(), _systems.end(), [index](const auto & system) { return system.type == index;  });
				if (it != _systems.end()) {
					removeModule(*it->ptr);
					_systems.erase(it);
				}
			}
			_toRemove.clear();
        }

    private:
        void resetTimers() {
            for (auto & system : _systems) {
                system.ptr->time.lastCall = putils::Timer::t_clock::now();
                system.ptr->time.timer.restart();
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
            static_assert(std::is_base_of<ISystem, T>(),
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

            const auto type = system->getType();

            auto & ret = *system;

			_toAdd.emplace_back(type, std::move(system));

            if (_first)
                updateSystemList();

            return ret;
        }

    public:
        template<typename ...Systems>
        void loadSystems(const char * pluginDir = nullptr, const char * creatorFunction = "getSystem", bool pluginsFirst = false) {
            if (pluginsFirst && pluginDir != nullptr)
                loadPlugins(pluginDir, creatorFunction);

            // Call "creatorFunc" in each plugin, passing myself as an EntityManager
            auto & em = static_cast<kengine::EntityManager &>(*this);

            putils::tuple_for_each(std::tuple < putils::meta::type<Systems>... > (),
                                  [this, &em](auto && type) {
                                      using System = putils_wrapped_type(type);
                                      createSystem<System>(em);
                                  }
            );

            if (!pluginsFirst && pluginDir != nullptr)
                loadPlugins(pluginDir, creatorFunction);
        }

    private:
		putils::PluginManager pm;

        template<typename String, typename String2> // This has to be a template so the static_cast isn't evaluated while parsing this header
        void loadPlugins(String && pluginDir, String2 && creatorFunction) {
            auto & em = static_cast<kengine::EntityManager &>(*this);

			const auto systems = pm.rescanDirectoryWithReturn<KENGINE_MAX_PLUGINS_LOADED_PER_FRAME, ISystem *>(pluginDir, creatorFunction, em);

            for (auto s : systems)
                addSystem(std::unique_ptr < kengine::ISystem > (s));
        }

    public:
        template<typename T>
        T & getSystem() {
            static_assert(std::is_base_of<ISystem, T>(), "Attempt to get something that isn't a System");
			for (auto & system : _systems)
				if (system.type == putils::meta::type<T>::index)
					return static_cast<T &>(*system.ptr);
			assert(false);
			return static_cast<T &>(*_systems[0].ptr);
        }

        template<typename T>
        const T & getSystem() const {
            static_assert(std::is_base_of<ISystem, T>(), "Attempt to get something that isn't a System");
			for (auto & system : _systems)
				if (system.type == putils::meta::type<T>::index)
					return static_cast<const T &>(*system.ptr);
			assert(false);
			return static_cast<const T &>(*_systems[0].ptr);
        }

        template<typename T>
        bool hasSystem() const noexcept {
            static_assert(std::is_base_of<ISystem, T>(), "Attempt to check something that isn't a System");
			for (auto & system : _systems)
				if (system.type == putils::meta::type<T>::index)
					return true;
			return false;
        }

        template<typename T>
        void removeSystem() noexcept {
            static_assert(std::is_base_of<ISystem, T>(), "Attempt to remove something that isn't a System");
			if (_inFrame)
				_toRemove.emplace_back(putils::meta::type<T>::index);
			else {
				const auto index = putils::meta::type<T>::index;
				const auto it = std::find_if(_systems.begin(), _systems.end(), [index](const auto & system) { return system.type == index; });
				if (it != _systems.end()) {
					removeModule(*it->ptr);
					_systems.erase(it);
				}
			}
        }

    public:
        void setSpeed(float speed) noexcept { _speed = speed; }
        float getSpeed() const noexcept { return _speed; }
        void pause() noexcept { _speed = 0; }
        void resume() noexcept { _speed = 1; }

    public:
        /*
         * Internal
         */

    protected:
        void registerEntity(Entity & e) noexcept {
            send(packets::RegisterEntity{ e });
        }

        void removeEntity(Entity & e) {
            send(packets::RemoveEntity{ e });
        }

    private:
        float _speed = 1;
		bool _inFrame = false;
        std::vector<std::pair<putils::meta::type_index, std::unique_ptr<ISystem>>> _toAdd;

        std::vector<putils::meta::type_index> _toRemove;

		std::vector<putils::function<void(), KENGINE_AFTER_SYSTEM_FUNCTION_SIZE>> _afterSystem;

	protected:
		struct SystemInfo {
			putils::meta::type_index type;
			std::unique_ptr<ISystem> ptr;
		};
        std::vector<SystemInfo> _systems;
    };
}
