#include "scriptLanguageHelper.hpp"

#include "kengine.hpp"
#include "string.hpp"
#include "with.hpp"
#include "function.hpp"

namespace kengine::scriptLanguageHelper {
	template<typename Func>
	using function = putils::function<Func, KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE>;

	template<typename Func, typename Func2>
	void init(Func && registerFunction, Func2 && registerType) noexcept {
		using CreateEntityFunc = function<void(Entity &)>;
		registerFunction("createEntity",
			function<Entity(const CreateEntityFunc &)>(
				[](const CreateEntityFunc & f) {
					return entities.create(FWD(f));
				}
			)
		);

		registerFunction("removeEntity",
			function<void(Entity)>(
				[](Entity go) { entities.remove(go); }
			)
		);
		registerFunction("removeEntityById",
			function<void(EntityID id)>(
				[](EntityID id) { entities.remove(id); }
			)
		);

		registerFunction("getEntity",
			function<Entity(EntityID id)>(
				[](EntityID id) { return entities.get(id); }
			)
		);

		using ForEachEntityFunc = function<void(Entity)>;
		registerFunction("forEachEntity",
			function<void(const ForEachEntityFunc &)>(
				[](const ForEachEntityFunc & f) {
					for (auto e : entities)
						f(e);
				}
			)
		);

		registerFunction("stopRunning",
			function<void()>(
				[] { stopRunning(); }
			)
		);

		registerType(putils::meta::type<Entity>{});
	}

	template<typename T, typename Func, typename Func2>
	void registerComponent(Func && registerEntityMember, Func2 && registerFunction) noexcept {
		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
		registerEntityMember(putils::string<128>("get%s", className),
			function<T & (Entity)>(
				[](Entity self) noexcept { return std::ref(self.get<T>()); }
			)
		);

		registerEntityMember(putils::string<128>("tryGet%s", className),
			function<const T * (Entity)>(
				[](Entity self) noexcept { return self.tryGet<T>(); }
			)
		);

		registerEntityMember(putils::string<128>("has%s", className),
			function<bool(Entity)>(
				[](Entity self) noexcept { return self.has<T>(); }
			)
		);

		registerEntityMember(putils::string<128>("attach%s", className),
			function<T & (Entity)>(
				[](Entity self) noexcept { return std::ref(self.attach<T>()); }
			)
		);

		registerEntityMember(putils::string<128>("detach%s", className),
			function<void(Entity)>(
				[](Entity self) noexcept { self.detach<T>(); }
			)
		);

		using ForEachEntityFunc = function<void(Entity, T &)>;
		registerFunction(putils::string<128>("forEachEntityWith%s", className),
			function<void(const ForEachEntityFunc &)>(
				[](const ForEachEntityFunc & f) {
					for (auto [e, t] : entities.with<T>())
						f(e, t);
				}
			)
		);
	}
}
