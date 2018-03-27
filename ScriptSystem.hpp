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
			crtp.registerFunction("getGameObjects", [this] { return std::ref(_em.getGameObjects()); });

			crtp.registerFunction("createEntity",
				[this](const std::string & type, const std::string & name, const sol::function & f) {
					return std::ref(_em.createEntity(type, name, f));
				}
			);

			crtp.registerFunction("createNamelessEntity",
				[this](const std::string & type, const sol::function & f) {
					return std::ref(_em.createEntity(type, f));
				}
			);

			crtp.registerFunction("removeEntity", [this](kengine::GameObject & go) { _em.removeEntity(go); });
            crtp.registerFunction("removeEntityByName", [this](const std::string & name) { _em.removeEntity(name); });

            crtp.registerFunction("getEntity", [this](const std::string & name) { return std::ref(_em.getEntity(name)); });
            crtp.registerFunction("hasEntity", [this](const std::string & name) { return _em.hasEntity(name); });

            crtp.registerFunction("getDeltaTime", [this] { return time.getDeltaTime(); });
            crtp.registerFunction("getFixedDeltaTime", [this] { return time.getFixedDeltaTime(); });
            crtp.registerFunction("getDeltaFrames", [this] { return time.getDeltaFrames(); });

            crtp.registerFunction("stopRunning", [this] { _em.running = false; });
            crtp.registerFunction("setSpeed", [this](double speed) { _em.setSpeed(speed); });
            crtp.registerFunction("getSpeed", [this] { return _em.getSpeed(); });
            crtp.registerFunction("isPaused", [this] { return isPaused(); });
            crtp.registerFunction("pause", [this] { _em.pause(); });
            crtp.registerFunction("resume", [this] { _em.resume(); });

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

            crtp.registerTypeInternal<T>();

            const auto sender = putils::concat("send", T::get_class_name());
			crtp.registerFunction(sender, [this](const T & packet) { send(packet); });

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
				const auto & comp = go->getComponent<CompType>();
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
				[this] { return std::ref(_em.getGameObjects<T>()); }
			);

			crtp.registerGameObjectMember(putils::concat("get", T::get_class_name()),
				[](kengine::GameObject & self) { return std::ref(self.getComponent<T>()); }
			);

			crtp.registerGameObjectMember(putils::concat("has", T::get_class_name()),
				[](kengine::GameObject & self) { return self.hasComponent<T>(); }
			);

			crtp.registerGameObjectMember(putils::concat("attach", T::get_class_name()),
				[](kengine::GameObject & self) { return std::ref(self.attachComponent<T>()); }
			);

			crtp.registerGameObjectMember(putils::concat("detach", T::get_class_name()),
				[](kengine::GameObject & self) { self.detachComponent<T>(); }
			);
		}


	private:
		kengine::EntityManager & _em;
        std::vector<std::string> _directories;
	};
}