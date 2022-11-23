#include "scriptLanguageHelper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// reflection
#include "reflection.hpp"

// putils
#include "string.hpp"
#include "with.hpp"
#include "function.hpp"

// kengine helpers
#include "helpers/isRunning.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// Reflection API for entt::handle
// We use entt::handle as entt::entity is a scalar and doesn't play well with scripting languages
#define refltype entt::handle
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

namespace kengine::scriptLanguageHelper {
	template<typename Func>
	using function = std::function<Func>;

	template<typename Func, typename Func2>
	void init(entt::registry & r, Func && registerFunction, Func2 && registerType) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Verbose, "scriptLanguageHelper::init", "Registering createEntity");

		registerFunction("createEntity",
			function<entt::handle()>(
				[&] { return entt::handle{ r, r.create() }; }
			)
		);

        kengine_log(r, Verbose, "scriptLanguageHelper::init", "Registering destroyEntity");
		registerFunction("destroyEntity",
			function<void(entt::handle)>(
				[](entt::handle e) { e.destroy(); }
			)
		);

        kengine_log(r, Verbose, "scriptLanguageHelper::init", "Registering forEachEntity");
		using ForEachEntityCallback = std::function<void(entt::handle)>;
		registerFunction("forEachEntity",
			function<void(const ForEachEntityCallback &)>(
				[&](const ForEachEntityCallback & f) {
					r.each([&](entt::entity e) {
						f({ r, e });
					});
				}
			)
		);

        kengine_log(r, Verbose, "scriptLanguageHelper::init", "Registering stopRunning");
		registerFunction("stopRunning",
			function<void()>(
				[&] { stopRunning(r); }
			)
		);

        kengine_log(r, Verbose, "scriptLanguageHelper::init", "Registering Entity type");
		registerType(putils::meta::type<entt::handle>{});
	}

	template<typename T, typename Func, typename Func2>
	void registerComponent(entt::registry & r, Func && registerEntityMember, Func2 && registerFunction) noexcept {
		KENGINE_PROFILING_SCOPE;

		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
        kengine_logf(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering component '%s'", className);

		if constexpr (!std::is_empty<T>()) {
			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering get");
			registerEntityMember(putils::string<128>("get%s", className).c_str(),
				 function<T &(entt::handle)>(
					 [](entt::handle self) noexcept { return std::ref(self.get<T>()); }
				 )
			);

			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering tryGet");
			registerEntityMember(putils::string<128>("tryGet%s", className).c_str(),
				 function<const T *(entt::handle)>(
					 [](entt::handle self) noexcept { return self.try_get<T>(); }
				 )
			);

			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering emplace");
			registerEntityMember(putils::string<128>("emplace%s", className).c_str(),
				 function<T & (entt::handle)>(
					 [](entt::handle self) noexcept { return std::ref(self.get_or_emplace<T>()); }
				 )
			);

			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering forEachEntityWith");
			using ForEachEntityFunc = function<void(entt::handle, T &)>;
			registerFunction(putils::string<128>("forEachEntityWith%s", className).c_str(),
				function<void(const ForEachEntityFunc &)>(
					[&](const ForEachEntityFunc & f) {
						for (const auto & [e, comp] : r.view<T>().each())
							f({ r, e }, comp);
					}
				)
			);
		}
		else {
			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering emplace");
			registerEntityMember(putils::string<128>("emplace%s", className).c_str(),
				 function<void(entt::handle)>(
					 [](entt::handle self) noexcept { self.emplace<T>(); }
				 )
			);

			kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering forEachEntityWith");
			using ForEachEntityFunc = function<void(entt::handle)>;
			registerFunction(putils::string<128>("forEachEntityWith%s", className).c_str(),
				function<void(const ForEachEntityFunc &)>(
					[&](const ForEachEntityFunc & f) {
						for (const auto & [e] : r.view<T>().each())
							f({ r, e });
					}
				)
			);
		}

        kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering has");
		registerEntityMember(putils::string<128>("has%s", className).c_str(),
			function<bool(entt::handle)>(
				[](entt::handle self) noexcept { return self.all_of<T>(); }
			)
		);

        kengine_log(r, Verbose, "scriptLanguageHelper::registerComponent", "Registering remove");
		registerEntityMember(putils::string<128>("remove%s", className).c_str(),
			function<void(entt::handle)>(
				[](entt::handle self) noexcept { self.remove<T>(); }
			)
		);
	}
}