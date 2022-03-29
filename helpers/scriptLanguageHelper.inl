#include "scriptLanguageHelper.hpp"

// kengine
#include "kengine.hpp"
#include "helpers/logHelper.hpp"

// putils
#include "string.hpp"
#include "with.hpp"
#include "function.hpp"

namespace kengine::scriptLanguageHelper {
	template<typename Func>
	using function = std::function<Func>;

	template<typename Func, typename Func2>
	void init(Func && registerFunction, Func2 && registerType) noexcept {
		using CreateEntityFunc = function<void(Entity &)>;
        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering createEntity");
		registerFunction("createEntity",
			function<Entity(const CreateEntityFunc &)>(
				[](const CreateEntityFunc & f) {
					return entities.create(FWD(f));
				}
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering removeEntity");
		registerFunction("removeEntity",
			function<void(Entity)>(
				[](Entity go) { entities -= go; }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering removeEntityById");
		registerFunction("removeEntityById",
			function<void(EntityID id)>(
				[](EntityID id) { entities -= id; }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering getEntity");
		registerFunction("getEntity",
			function<Entity(EntityID id)>(
				[](EntityID id) { return entities[id]; }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering forEachEntity");
		using ForEachEntityFunc = function<void(Entity)>;
		registerFunction("forEachEntity",
			function<void(const ForEachEntityFunc &)>(
				[](const ForEachEntityFunc & f) {
					for (auto e : entities)
						f(e);
				}
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering stopRunning");
		registerFunction("stopRunning",
			function<void()>(
				[] { stopRunning(); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::init", "Registering Entity type");
		registerType(putils::meta::type<Entity>{});
	}

	template<typename T, typename Func, typename Func2>
	void registerComponent(Func && registerEntityMember, Func2 && registerFunction) noexcept {
		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
        kengine_logf(Verbose, "scriptLanguageHelper::registerComponent", "Registering component '%s'", className);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering get");
		registerEntityMember(putils::string<128>("get%s", className),
			function<T & (Entity)>(
				[](Entity self) noexcept { return std::ref(self.get<T>()); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering tryGet");
		registerEntityMember(putils::string<128>("tryGet%s", className),
			function<const T * (Entity)>(
				[](Entity self) noexcept { return self.tryGet<T>(); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering has");
		registerEntityMember(putils::string<128>("has%s", className),
			function<bool(Entity)>(
				[](Entity self) noexcept { return self.has<T>(); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering attach");
		registerEntityMember(putils::string<128>("attach%s", className),
			function<T & (Entity)>(
				[](Entity self) noexcept { return std::ref(self.attach<T>()); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering detach");
		registerEntityMember(putils::string<128>("detach%s", className),
			function<void(Entity)>(
				[](Entity self) noexcept { self.detach<T>(); }
			)
		);

        kengine_log(Verbose, "scriptLanguageHelper::registerComponent", "Registering forEachEntityWith");
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
