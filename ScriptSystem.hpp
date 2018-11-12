#pragma once

#include "System.hpp"
#include "EntityManager.hpp"
#include "with.hpp"

namespace kengine {
	template<typename CRTP, typename CompType, typename ...Datapackets>
	class ScriptSystem : public kengine::System<CRTP, Datapackets...> {
	public:
		ScriptSystem(EntityManager & em) : System(em), _em(em) {}

	public:
		void init() noexcept {
			auto & _ = static_cast<CRTP &>(*this);

			_.registerFunction("createEntity",
				std::function<Entity(const std::function<void(Entity &)> &)>(
					[this](const std::function<void(Entity &)> & f) {
						return _em.createEntity(FWD(f));
					}
				)
			);

			_.registerFunction("removeEntity",
				std::function<void(Entity &)>(
					[this](Entity & go) { _em.removeEntity(go); }
				)
			);
			_.registerFunction("removeEntityById",
				std::function<void(Entity::ID id)>(
					[this](Entity::ID id) { _em.removeEntity(id); }
				)
			);

			_.registerFunction("getEntity",
				std::function<Entity(Entity::ID id)>(
					[this](Entity::ID id) { return _em.getEntity(id); }
				)
			);

			_.registerFunction("getDeltaTime",
				std::function<float()>(
					[this] { return this->time.getDeltaTime().count(); }
				)
			);
			_.registerFunction("getFixedDeltaTime",
				std::function<float()>(
					[this] { return this->time.getFixedDeltaTime().count(); }
				)
			);
			_.registerFunction("getDeltaFrames",
				std::function<float()>(
					[this] { return this->time.getDeltaFrames(); }
					)
			);

			_.registerFunction("stopRunning",
				std::function<void()>(
					[this] { _em.running = false; }
				)
			);
			_.registerFunction("setSpeed",
				std::function<void(float)>(
					[this](float speed) { _em.setSpeed(speed); }
				)
			);
			_.registerFunction("getSpeed",
				std::function<float()>(
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

			_.registerFunction("runAfterSystem",
				std::function<void(const std::function<void()> &)>(
					[this](const std::function<void()> & func) { _em.runAfterSystem(func); }
				)
			);

			registerType<Entity>();
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
			registerComponent<T>();
		}

		// System methods
	public:
		void execute() final {
			{ pmeta_with(static_cast<CRTP &>(*this)) {
				for (const auto & [go, comp] : _em.getEntities<CompType>()) {
					_.setSelf(go);
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
				_.registerEntityMember(putils::concat("get", T::get_class_name()),
					std::function<T &(kengine::Entity &)>(
						[](kengine::Entity & self) { return std::ref(self.get<T>()); }
						)
				);

				_.registerEntityMember(putils::concat("has", T::get_class_name()),
					std::function<bool(kengine::Entity &)>(
						[](kengine::Entity & self) { return self.has<T>(); }
						)
				);

				_.registerEntityMember(putils::concat("attach", T::get_class_name()),
					std::function<T &(kengine::Entity &)>(
						[](kengine::Entity & self) { return std::ref(self.attach<T>()); }
						)
				);

				_.registerEntityMember(putils::concat("detach", T::get_class_name()),
					std::function<void(kengine::Entity &)>(
						[](kengine::Entity & self) { self.detach<T>(); }
						)
				);
			}}
		}

	private:
		EntityManager & _em;
	};
}
