#pragma

// entt
#include <entt/entity/fwd.hpp>

// nlohmann
#include <nlohmann/json.hpp>

namespace kengine::jsonHelper {
	KENGINE_CORE_EXPORT void loadEntity(const nlohmann::json & entityJSON, entt::handle e) noexcept;
	KENGINE_CORE_EXPORT nlohmann::json saveEntity(entt::const_handle e) noexcept;
}