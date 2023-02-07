#include "load_from_json.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine helpers
#include "kengine/helpers/json_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::load_from_json, T>::function(const nlohmann::json & json_entity, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto it = json_entity.find(putils::reflection::get_class_name<T>());
		if (it == json_entity.end())
			return;

		if constexpr (!std::is_empty<T>()) {
			T comp;
			putils::reflection::from_json(*it, comp);
			e.emplace_or_replace<T>(std::move(comp));
		}
		else
			e.emplace<T>();
	}
}
