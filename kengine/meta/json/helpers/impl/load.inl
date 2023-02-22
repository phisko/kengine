#include "load.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine core
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<json::load, T>::function(const nlohmann::json & json_entity, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::json::load", "Loading [%u]'s %s from JSON", e.entity(), putils::reflection::get_class_name<T>());

		const auto it = json_entity.find(putils::reflection::get_class_name<T>());
		if (it == json_entity.end()) {
			kengine_log(*e.registry(), very_verbose, "meta::json::load", "Component not found in JSON");
			return;
		}

		if constexpr (!std::is_empty<T>()) {
			T comp;
			putils::reflection::from_json(*it, comp);
			e.emplace_or_replace<T>(std::move(comp));
		}
		else {
			kengine_log(*e.registry(), very_verbose, "meta::json::load", "Component is empty, not parsing anything");
			e.emplace<T>();
		}
	}
}
