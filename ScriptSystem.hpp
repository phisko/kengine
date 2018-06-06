#pragma once

#include "System.hpp"
#include "EntityManager.hpp"
#include "with.hpp"

namespace kengine {
	template<typename CRTP, typename CompType, typename ...Datapackets>
	class ScriptSystem : public kengine::System<CRTP, Datapackets...> {
	public:
		ScriptSystem(kengine::EntityManager & em) : _em(em) {}

	public:
		void init() noexcept {
			{ pmeta_with(static_cast<CRTP &>(*this)) {
				_.registerFunction("getGameObjects",
					std::function<const std::vector<kengine::GameObject *> &()>(
						[this] { return std::ref(_em.getGameObjects()); }
						)
				);

				_.registerFunction("createEntity",
					std::function<kengine::GameObject &(const std::string &, const std::string &, const std::function<void(kengine::GameObject &)> &)>(
						[this](const std::string & type, const std::string & name, auto && f) {
					return std::ref(_em.createEntity(type, name, FWD(f)));
				}
						)
				);

				_.registerFunction("createNamelessEntity",
					std::function<kengine::GameObject &(const std::string &, const std::function<void(kengine::GameObject &)> &)>(
						[this](const std::string & type, auto && f) {
					return std::ref(_em.createEntity(type, FWD(f)));
				}
						)
				);

				_.registerFunction("removeEntity",
					std::function<void(kengine::GameObject &)>(
						[this](kengine::GameObject & go) { _em.removeEntity(go); }
						)
				);
				_.registerFunction("removeEntityByName",
					std::function<void(const std::string &)>(
						[this](const std::string & name) { _em.removeEntity(name); }
						)
				);

				_.registerFunction("enableEntity",
					std::function<void(kengine::GameObject &)>(
						[this](kengine::GameObject & go) { _em.enableEntity(go); }
						)
				);
				_.registerFunction("enableEntityByName",
					std::function<void(const std::string &)>(
						[this](const std::string & name) { _em.enableEntity(name); }
						)
				);

				_.registerFunction("disableEntity",
					std::function<void(kengine::GameObject &)>(
						[this](kengine::GameObject & go) { _em.disableEntity(go); }
						)
				);
				_.registerFunction("disableEntityByName",
					std::function<void(const std::string &)>(
						[this](const std::string & name) { _em.disableEntity(name); }
						)
				);

				_.registerFunction("isEntityEnabled",
					std::function<bool(kengine::GameObject &)>(
						[this](kengine::GameObject & go) { return _em.isEntityEnabled(go); }
						)
				);
				_.registerFunction("isEntityEnabledByName",
					std::function<bool(const std::string &)>(
						[this](const std::string & name) { return _em.isEntityEnabled(name); }
						)
				);

				_.registerFunction("getEntity",
					std::function<kengine::GameObject &(const std::string &)>(
						[this](const std::string & name) { return std::ref(_em.getEntity(name)); }
						)
				);
				_.registerFunction("hasEntity",
					std::function<bool(const std::string &)>(
						[this](const std::string & name) { return _em.hasEntity(name); }
						)
				);

				_.registerFunction("getDeltaTime",
					std::function<putils::Timer::t_duration()>(
						[this] { return this->time.getDeltaTime(); }
						)
				);
				_.registerFunction("getFixedDeltaTime",
					std::function<putils::Timer::t_duration()>(
						[this] { return this->time.getFixedDeltaTime(); }
						)
				);
				_.registerFunction("getDeltaFrames",
					std::function<double()>(
						[this] { return this->time.getDeltaFrames(); }
						)
				);

				_.registerFunction("stopRunning",
					std::function<void()>(
						[this] { _em.running = false; }
						)
				);
				_.registerFunction("setSpeed",
					std::function<void(double)>(
						[this](double speed) { _em.setSpeed(speed); }
						)
				);
				_.registerFunction("getSpeed",
					std::function<double()>(
						[this] { return _em.getSpeed(); }
						)
				);
				_.registerFunction("isPaused",
					std::function<bool()>(
						[this] { return this->isPaused(); }
						)
				);
				_.registerFunction("pause",
					std::function<void()>(
						[this] { _em.pause(); }
						)
				);
				_.registerFunction("resume",
					std::function<void()>(
						[this] { _em.resume(); }
						)
				);

				_.registerFunction("save",
					std::function<void(const std::string &)>(
						[this](const std::string & file) { _em.save(file); }
						)
				);
				_.registerFunction("load",
					std::function<void(const std::string &)>(
						[this](const std::string & file) { _em.load(file); }
						)
				);

				_.registerFunction("onLoad",
					std::function<void(const std::function<void()> &)>(
						[this](const std::function<void()> & func) { _em.onLoad(func); }
						)
				);

				_.registerFunction("runAfterSystem",
					std::function<void(const std::function<void()> &)>(
						[this](const std::function<void()> & func) { _em.runAfterSystem(func); }
						)
				);

			}}

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
			{ pmeta_with(static_cast<CRTP &>(*this)) {
#ifdef _WIN32
				_.registerTypeInternal<T>();
#else
				_.template registerTypeInternal<T>();
#endif

				const auto sender = putils::concat("send", T::get_class_name());
#ifdef _WIN32
				_.registerFunction(sender,
					std::function<void(const T &)>(
						[this](const T & packet) { this->send(packet); }
						)
				);
#else
				_.template registerFunction(sender,
					std::function<void(const T &)>(
						[this](const T & packet) { this->send(packet); }
						)
				);
#endif
			}}
			if constexpr (kengine::is_component<T>::value)
				registerComponent<T>();
		}

	public:
		template<typename String>
		void addScriptDirectory(String && dir) noexcept {
			try {
				putils::Directory d(dir);
				_directories.emplace_back(FWD(dir));
			}
			catch (const std::runtime_error & e) {
				std::cerr << e.what() << std::endl;
			}
		}

		template<typename String>
		void removeScriptDirectory(String && dir) noexcept {
			const auto it = std::find(_directories.begin(), _directories.end(), dir);
			if (it != _directories.end())
				_directories.erase(it);
		}

		// System methods
	public:
		void execute() final {
			executeDirectories();
			executeScriptedObjects();
		}

	private:
		void executeDirectories() noexcept {
			{ pmeta_with(static_cast<CRTP &>(*this)) {
				for (const auto & dir : _directories) {
					try {
						putils::Directory d(dir);

						d.for_each([&_](const putils::Directory::File & f) {
							if (!f.isDirectory)
								_.executeScript(f.fullPath);
						});
					}
					catch (const std::runtime_error & e) {
						std::cerr << e.what() << std::endl;
					}
				}
			}}
		}

		void executeScriptedObjects() noexcept {
			{ pmeta_with(static_cast<CRTP &>(*this)) {
				for (const auto go : _em.getGameObjects<CompType>()) {
#ifdef _WIN32
					const auto & comp = go->getComponent<CompType>();
#else
					const auto & comp = go->template getComponent<CompType>();
#endif
					_.setSelf(*go);
					for (const auto & s : comp.getScripts())
						_.executeScript(s);
				}
				_.unsetSelf();
			}}
		}

	private:
		template<typename T>
		void registerComponent() noexcept {
			{ pmeta_with(static_cast<CRTP &>(*this)) {
				_.registerFunction(putils::concat("getGameObjectsWith", T::get_class_name()),
					std::function<const std::vector<kengine::GameObject *> &()>(
						[this] { return std::ref(_em.getGameObjects<T>()); }
						)
				);

				_.registerGameObjectMember(putils::concat("get", T::get_class_name()),
					std::function<T &(kengine::GameObject &)>(
						[](kengine::GameObject & self) { return std::ref(self.getComponent<T>()); }
						)
				);

				_.registerGameObjectMember(putils::concat("has", T::get_class_name()),
					std::function<bool(kengine::GameObject &)>(
						[](kengine::GameObject & self) { return self.hasComponent<T>(); }
						)
				);

				_.registerGameObjectMember(putils::concat("attach", T::get_class_name()),
					std::function<T &(kengine::GameObject &)>(
						[](kengine::GameObject & self) { return std::ref(self.attachComponent<T>()); }
						)
				);

				_.registerGameObjectMember(putils::concat("detach", T::get_class_name()),
					std::function<void(kengine::GameObject &)>(
						[](kengine::GameObject & self) { self.detachComponent<T>(); }
						)
				);
			}}
		}

	private:
		kengine::EntityManager & _em;
		std::vector<std::string> _directories;
	};
}
