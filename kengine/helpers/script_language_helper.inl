#include "script_language_helper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/string.hpp"
#include "putils/with.hpp"
#include "putils/function.hpp"

// kengine helpers
#include "kengine/helpers/is_running.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// Reflection API for entt::handle
// We use entt::handle as entt::entity is a scalar and doesn't play well with scripting languages
#define refltype entt::handle
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

namespace kengine::script_language_helper {
	template<typename Func>
	using function = std::function<Func>;

	template<typename Func, typename Func2>
	void init(entt::registry & r, Func && register_function, Func2 && register_type) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "script_language_helper::init", "Registering create_entity");

		register_function(
			"create_entity",
			function<entt::handle()>(
				[&] { return entt::handle{ r, r.create() }; }
			)
		);

		kengine_log(r, verbose, "script_language_helper::init", "Registering destroy_entity");
		register_function(
			"destroy_entity",
			function<void(entt::handle)>(
				[](entt::handle e) { e.destroy(); }
			)
		);

		kengine_log(r, verbose, "script_language_helper::init", "Registering for_each_entity");
		using ForEachEntityCallback = std::function<void(entt::handle)>;
		register_function(
			"for_each_entity",
			function<void(const ForEachEntityCallback &)>(
				[&](const ForEachEntityCallback & f) {
					r.each([&](entt::entity e) {
						f({ r, e });
					});
				}
			)
		);

		kengine_log(r, verbose, "script_language_helper::init", "Registering stop_running");
		register_function(
			"stop_running",
			function<void()>(
				[&] {
					stop_running(r);
				}
			)
		);

		kengine_log(r, verbose, "script_language_helper::init", "Registering Entity type");
		register_type(putils::meta::type<entt::handle>{});
	}

	template<typename T, typename Func, typename Func2>
	void register_component(entt::registry & r, Func && register_entity_member, Func2 && register_function) noexcept {
		KENGINE_PROFILING_SCOPE;

		static_assert(putils::reflection::has_class_name<T>());

		const auto className = putils::reflection::get_class_name<T>();
		kengine_logf(r, verbose, "script_language_helper::register_component", "Registering component '%s'", className);

		if constexpr (!std::is_empty<T>()) {
			kengine_log(r, verbose, "script_language_helper::register_component", "Registering get");
			register_entity_member(
				putils::string<128>("get_%s", className).c_str(),
				function<T &(entt::handle)>(
					[](entt::handle self) noexcept {
						return std::ref(self.get<T>());
					}
				)
			);

			kengine_log(r, verbose, "script_language_helper::register_component", "Registering try_get");
			register_entity_member(
				putils::string<128>("try_get_%s", className).c_str(),
				function<const T *(entt::handle)>(
					[](entt::handle self) noexcept {
						return self.try_get<T>();
					}
				)
			);

			kengine_log(r, verbose, "script_language_helper::register_component", "Registering emplace");
			register_entity_member(
				putils::string<128>("emplace_%s", className).c_str(),
				function<T &(entt::handle)>(
					[](entt::handle self) noexcept {
						return std::ref(self.get_or_emplace<T>());
					}
				)
			);

			kengine_log(r, verbose, "script_language_helper::register_component", "Registering for_each_entity_with");
			using ForEachEntityFunc = function<void(entt::handle, T &)>;
			register_function(
				putils::string<128>("for_each_entity_with_%s", className).c_str(),
				function<void(const ForEachEntityFunc &)>(
					[&](const ForEachEntityFunc & f) {
						for (const auto & [e, comp] : r.view<T>().each())
							f({ r, e }, comp);
					}
				)
			);
		}
		else {
			kengine_log(r, verbose, "script_language_helper::register_component", "Registering emplace");
			register_entity_member(
				putils::string<128>("emplace_%s", className).c_str(),
				function<void(entt::handle)>(
					[](entt::handle self) noexcept {
						self.emplace<T>();
					}
				)
			);

			kengine_log(r, verbose, "script_language_helper::register_component", "Registering for_each_entity_with");
			using ForEachEntityFunc = function<void(entt::handle)>;
			register_function(
				putils::string<128>("for_each_entity_with_%s", className).c_str(),
				function<void(const ForEachEntityFunc &)>(
					[&](const ForEachEntityFunc & f) {
						for (const auto & [e] : r.view<T>().each())
							f({ r, e });
					}
				)
			);
		}

		kengine_log(r, verbose, "script_language_helper::register_component", "Registering has");
		register_entity_member(
			putils::string<128>("has_%s", className).c_str(),
			function<bool(entt::handle)>(
				[](entt::handle self) noexcept {
					return self.all_of<T>();
				}
			)
		);

		kengine_log(r, verbose, "script_language_helper::register_component", "Registering remove");
		register_entity_member(
			putils::string<128>("remove_%s", className).c_str(),
			function<void(entt::handle)>(
				[](entt::handle self) noexcept {
					self.remove<T>();
				}
			)
		);
	}
}