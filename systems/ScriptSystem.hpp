#pragma once

#include "EntityManager.hpp"
#include "concat.hpp"
#include "with.hpp"

namespace kengine::ScriptSystem {
	template<typename Func, typename Func2>
	void init(EntityManager & em, Func && registerFunction, Func2 && registerType) {
		registerFunction("createEntity",
			std::function<Entity(const std::function<void(Entity &)> &)>(
				[&](const std::function<void(Entity &)> & f) {
					return em.createEntity(FWD(f));
				}
			)
		);

		registerFunction("removeEntity",
			std::function<void(Entity &)>(
				[&](Entity & go) { em.removeEntity(go); }
			)
		);
		registerFunction("removeEntityById",
			std::function<void(Entity::ID id)>(
				[&](Entity::ID id) { em.removeEntity(id); }
			)
		);

		registerFunction("getEntity",
			std::function<Entity(Entity::ID id)>(
				[&](Entity::ID id) { return em.getEntity(id); }
			)
		);

		registerFunction("stopRunning",
			std::function<void()>(
				[&] { em.running = false; }
			)
		);

		registerType(putils::meta::type<Entity>{});
	}

	template<typename Comp, typename Func, typename Func2>
	void execute(EntityManager & em, Func && setSelf, Func2 && executeComp) {
		for (auto & [e, comp] : em.getEntities<Comp>()) {
			setSelf(e);
			executeComp(comp);
		}
	}

	template<typename T, typename Func>
	void registerComponent(Func && registerEntityMember) {
		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
		registerEntityMember(putils::concat("get", className),
			std::function<T & (Entity &)>(
				[](Entity & self) { return std::ref(self.get<T>()); }
				)
		);

		registerEntityMember(putils::concat("has", className),
			std::function<bool(Entity &)>(
				[](Entity & self) { return self.has<T>(); }
				)
		);

		registerEntityMember(putils::concat("attach", className),
			std::function<T & (Entity &)>(
				[](Entity & self) { return std::ref(self.attach<T>()); }
				)
		);

		registerEntityMember(putils::concat("detach", className),
			std::function<void(Entity &)>(
				[](Entity & self) { self.detach<T>(); }
				)
		);
	}
}
