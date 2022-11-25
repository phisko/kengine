#pragma

// entt
#include <entt/entity/fwd.hpp>

// nlohmann
#include <nlohmann/json.hpp>

namespace kengine::json_helper {
	KENGINE_CORE_EXPORT void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept;
	KENGINE_CORE_EXPORT nlohmann::json save_entity(entt::const_handle e) noexcept;
}