#include "save_to_json.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine helpers
#include "kengine/helpers/json_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	nlohmann::json meta_component_implementation<meta::save_to_json, T>::function(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		if constexpr (std::is_empty<T>())
			return nlohmann::json{};
		else {
			const auto comp = e.try_get<T>();
			if (!comp)
				return nlohmann::json{};
			return putils::reflection::to_json(*comp);
		}
	}
}