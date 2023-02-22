#pragma once

// entt
#include <entt/entity/fwd.hpp>

// nlohmann
#include <nlohmann/json.hpp>

namespace kengine::meta::json {
	KENGINE_META_JSON_EXPORT nlohmann::json save_entity(entt::const_handle e) noexcept;
}