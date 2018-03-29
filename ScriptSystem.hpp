#pragma once

#include "System.hpp"
#include "EntityManager.hpp"

namespace kengine {
	template<typename CRTP, typename CompType, typename ...Datapackets>
	class ScriptSystem : public kengine::System<CRTP, Datapackets...> {
	public:
		ScriptSystem(kengine::EntityManager & em) : _em(em) {}

	public:
		void init() noexcept {
			auto & crtp = static_cast<CRTP &>(*this);
			crtp.registerFunction("getGameObjects",
				std::function<const std::vector<kengine::GameObject*> &()>(
					[this] { return std::ref(_em.getGameObjects()); }
				)
			);

			crtp.registerFunction("createEntity",
				std::function<kengine::GameObject &(const std::string &, const std::string &, const std::function<void(kengine::GameObject &)> &)>(
					[this](const std::string & type, const std::string & name, auto && f) {
						return std::ref(_em.createEntity(type, name, FWD(f)));
					}
				)
			);

			crtp.registerFunction("createNamelessEntity",
				std::function<kengine::GameObject &(const std::string &, const std::function<void(kengine::GameObject &)> &)>(
					[this](const std::string & type, auto && f) {
						return std::ref(_em.createEntity(type, FWD(f)));
					}
				)
			);

			crtp.registerFunction("removeEntity",
				std::function<void(kengine::GameObject &)>(
					[this](kengine::GameObject & go) { _em.removeEntity(go); }
				)
			);
            crtp.registerFunction("removeEntityByName",
				std::function<void(const std::string &)>(
					[this](const std::string & name) { _em.removeEntity(name); }
				)
			);

            crtp.registerFunction("getEntity",
				std::function<kengine::GameObject &(const std::string &)>(
					[this](const std::string & name) { return std::ref(_em.getEntity(name)); }
				)
			);
            crtp.registerFunction("hasEntity",
				std::function<bool(const std::string &)>(
					[this](const std::string & name) { return _em.hasEntity(name); }
				)
			);

            crtp.registerFunction("getDeltaTime",
				std::function<putils::Timer::t_duration()>(
					[this] { return this->time.getDeltaTime(); }
				)
			);
            crtp.registerFunction("getFixedDeltaTime",
				std::function<putils::Timer::t_duration()>(
					[this] { return this->time.getFixedDeltaTime(); }
				)
			);
            crtp.registerFunction("getDeltaFrames",
				std::function<double()>(
					[this] { return this->time.getDeltaFrames(); }
				)
			);

            crtp.registerFunction("stopRunning",
				std::function<void()>(
					[this] { _em.running = false; }
				)
			);
            crtp.registerFunction("setSpeed",
				std::function<void(double)>(
					[this](double speed) { _em.setSpeed(speed); }
				)
			);
            crtp.registerFunction("getSpeed",
				std::function<double()>(
					[this] { return _em.getSpeed(); }
				)
			);
            crtp.registerFunction("isPaused",
				std::function<bool()>(
					[this] { return this->isPaused(); }
				)
			);
            crtp.registerFunction("pause",
				std::function<void()>(
					[this] { _em.pause(); }
				)
			);
            crtp.registerFunction("resume",
				std::function<void()>(
					[this] { _em.resume(); }
				)
			);

            registerType<kengine::GameObject>();
		}

	public:
        template<typename ...Types>
        void registerTypes() noexcept {
            pmeta::tuple_for_each(std::make_tuple(pmeta::type<Types>()...),
                                  [this](auto && t) {
                                      using Type = pmeta_wrapped(t);
                                      registerType<Type>();
                                  }
            );
        }

        template<typename T>
        void registerType() noexcept {
			auto & crtp = static_cast<CRTP &>(*this);

#ifdef _WIN32
            crtp.registerTypeInternal<T>();
#else
            crtp.template registerTypeInternal<T>();
#endif

            const auto sender = putils::concat("send", T::get_class_name());
#ifdef _WIN32
			crtp.registerFunction(sender,
				std::function<void(const T &)>(
					[this](const T & packet) { this->send(packet); }
				)
			);
#else
			crtp.template registerFunction(sender,
				std::function<void(const T &)>(
					[this](const T & packet) { this->send(packet); }
				)
			);
#endif

            if constexpr (kengine::is_component<T>::value)
                registerComponent<T>();
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

        // System methods
    public:
        void execute() final {
            executeDirectories();
            executeScriptedObjects();
        }

	private:
        void executeDirectories() noexcept {
			auto & crtp = static_cast<CRTP &>(*this);
            for (const auto & dir : _directories) {
                try {
                    putils::Directory d(dir);

                    d.for_each([this, &crtp](const putils::Directory::File & f) {
							if (!f.isDirectory)
								crtp.executeScript(f.fullPath);
						}
                    );
                } catch (const std::runtime_error & e) {
                    std::cerr << e.what() << std::endl;
                }
            }
        }

		void executeScriptedObjects() noexcept {
			auto & crtp = static_cast<CRTP &>(*this);

			for (const auto go : _em.getGameObjects<CompType>()) {
#ifdef _WIN32
				const auto & comp = go->getComponent<CompType>();
#else
				const auto & comp = go->template getComponent<CompType>();
#endif
				crtp.setSelf(*go);
				for (const auto & s : comp.getScripts())
					crtp.executeScript(s);
			}
			crtp.unsetSelf();
		}

	private:
		template<typename T>
		void registerComponent() noexcept {
			auto & crtp = static_cast<CRTP &>(*this);

			crtp.registerFunction(putils::concat("getGameObjectsWith", T::get_class_name()),
				std::function<const std::vector<kengine::GameObject *> &()>(
					[this] { return std::ref(_em.getGameObjects<T>()); }
				)
			);

			crtp.registerGameObjectMember(putils::concat("get", T::get_class_name()),
				std::function<T &(kengine::GameObject &)>(
					[](kengine::GameObject & self) { return std::ref(self.getComponent<T>()); }
				)
			);

			crtp.registerGameObjectMember(putils::concat("has", T::get_class_name()),
				std::function<bool(kengine::GameObject &)>(
					[](kengine::GameObject & self) { return self.hasComponent<T>(); }
				)
			);

			crtp.registerGameObjectMember(putils::concat("attach", T::get_class_name()),
				std::function<T &(kengine::GameObject &)>(
					[](kengine::GameObject & self) { return std::ref(self.attachComponent<T>()); }
				)
			);

			crtp.registerGameObjectMember(putils::concat("detach", T::get_class_name()),
				std::function<void(kengine::GameObject &)>(
					[](kengine::GameObject & self) { self.detachComponent<T>(); }
				)
			);
		}


	private:
		kengine::EntityManager & _em;
        std::vector<std::string> _directories;
	};
}