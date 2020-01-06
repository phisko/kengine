#pragma once

#include "EntityManager.hpp"
#include "string.hpp"
#include "with.hpp"
#include "function.hpp"

#ifndef KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE
# define KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE 64
#endif

namespace kengine::ScriptSystem {
	template<typename Func>
	using function = putils::function<Func, KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE>;

	template<typename Func, typename Func2>
	void init(EntityManager & em, Func && registerFunction, Func2 && registerType) {
		registerFunction("createEntity",
			function<Entity(const function<void(Entity &)> &)>(
				[&](const function<void(Entity &)> & f) {
					return em.createEntity(FWD(f));
				}
			)
		);

		registerFunction("removeEntity",
			function<void(Entity &)>(
				[&](Entity & go) { em.removeEntity(go); }
			)
		);
		registerFunction("removeEntityById",
			function<void(Entity::ID id)>(
				[&](Entity::ID id) { em.removeEntity(id); }
			)
		);

		registerFunction("getEntity",
			function<Entity(Entity::ID id)>(
				[&](Entity::ID id) { return em.getEntity(id); }
			)
		);

		registerFunction("stopRunning",
			function<void()>(
				[&] { em.running = false; }
			)
		);

		registerType(putils::meta::type<Entity>{});
	}

	template<typename T, typename Func>
	void registerComponent(Func && registerEntityMember) {
		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
		registerEntityMember(putils::string<128>("get%s", className),
			function<T & (Entity &)>(
				[](Entity & self) { return std::ref(self.get<T>()); }
			)
		);

		registerEntityMember(putils::string<128>("has%s", className),
			function<bool(Entity &)>(
				[](Entity & self) { return self.has<T>(); }
			)
		);

		registerEntityMember(putils::string<128>("attach%s", className),
			function<T & (Entity &)>(
				[](Entity & self) { return std::ref(self.attach<T>()); }
			)
		);

		registerEntityMember(putils::string<128>("detach%s", className),
			function<void(Entity &)>(
				[](Entity & self) { self.detach<T>(); }
			)
		);
	}
}
