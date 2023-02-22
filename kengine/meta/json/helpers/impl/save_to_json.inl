#include "save_to_json.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine core
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	nlohmann::json meta_component_implementation<save_to_json, T>::function(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::save_to_json", "Saving [%u]'s %s to JSON", e.entity(), putils::reflection::get_class_name<T>());

		if constexpr (std::is_empty<T>()) {
			kengine_log(*e.registry(), very_verbose, "meta::load_from_json", "Component is empty");
			return nlohmann::json{};
		}
		else {
			const auto comp = e.try_get<T>();
			if (!comp) {
				kengine_log(*e.registry(), very_verbose, "meta::load_from_json", "Component not found in JSON");
				return nlohmann::json{};
			}
			return putils::reflection::to_json(*comp);
		}
	}
}