#include "register_component.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

#include "entt_handle_reflection.hpp"

namespace kengine::scripting {
	template<typename T, typename Func, typename Func2>
	void register_component(entt::registry & r, Func && register_entity_member, Func2 && register_function) noexcept {
		KENGINE_PROFILING_SCOPE;

		static_assert(putils::reflection::has_class_name<T>());

		const auto class_name = putils::reflection::get_class_name<T>();
		kengine_logf(r, verbose, "script_language_helper", "Registering component '%s'", class_name);

		if constexpr (!std::is_empty<T>()) {
			kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member get_%s", class_name);
			register_entity_member(
				putils::string<128>("get_%s", class_name).c_str(),
				std::function<T &(entt::handle)>(
					[](entt::handle self) noexcept {
						return std::ref(self.get<T>());
					}
				)
			);

			kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member try_get_%s", class_name);
			register_entity_member(
				putils::string<128>("try_get_%s", class_name).c_str(),
				std::function<const T *(entt::handle)>(
					[](entt::handle self) noexcept {
						return self.try_get<T>();
					}
				)
			);

			kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member emplace_%s", class_name);
			register_entity_member(
				putils::string<128>("emplace_%s", class_name).c_str(),
				std::function<T &(entt::handle)>(
					[](entt::handle self) noexcept {
						return std::ref(self.get_or_emplace<T>());
					}
				)
			);

			kengine_logf(r, verbose, "script_language_helper", "Registering function for_each_entity_with_%s", class_name);
			using ForEachEntityFunc = std::function<void(entt::handle, T &)>;
			register_function(
				putils::string<128>("for_each_entity_with_%s", class_name).c_str(),
				std::function<void(const ForEachEntityFunc &)>(
				[&](const ForEachEntityFunc & f) {
					for (const auto & [e, comp] : r.view<T>().each())
						f({ r, e }, comp);
				}
			)
			);
		}
		else {
			kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member emplace_%s", class_name);
			register_entity_member(
				putils::string<128>("emplace_%s", class_name).c_str(),
				std::function<void(entt::handle)>(
					[](entt::handle self) noexcept {
						self.emplace<T>();
					}
				)
			);

			kengine_logf(r, verbose, "script_language_helper", "Registering function for_each_entity_with_%s", class_name);
			using ForEachEntityFunc = std::function<void(entt::handle)>;
			register_function(
				putils::string<128>("for_each_entity_with_%s", class_name).c_str(),
				std::function<void(const ForEachEntityFunc &)>(
				[&](const ForEachEntityFunc & f) {
					for (const auto & [e] : r.view<T>().each())
						f({ r, e });
				}
			)
			);
		}

		kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member has_%s", class_name);
		register_entity_member(
			putils::string<128>("has_%s", class_name).c_str(),
			std::function<bool(entt::handle)>(
				[](entt::handle self) noexcept {
					return self.all_of<T>();
				}
			)
		);

		kengine_logf(r, verbose, "script_language_helper", "Registering entt::handle member remove_%s", class_name);
		register_entity_member(
			putils::string<128>("remove_%s", class_name).c_str(),
			std::function<void(entt::handle)>(
				[](entt::handle self) noexcept {
					self.remove<T>();
				}
			)
		);
	}
}