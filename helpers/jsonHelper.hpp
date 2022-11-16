#pragma

#include "kengine.hpp"

// nlohmann
#include <nlohmann/json.hpp>

namespace kengine::jsonHelper {
	KENGINE_CORE_EXPORT Entity createEntity(const nlohmann::json & entityJSON) noexcept;
	KENGINE_CORE_EXPORT void loadEntity(const nlohmann::json & entityJSON, Entity & e) noexcept;
	KENGINE_CORE_EXPORT nlohmann::json saveEntity(const Entity & e) noexcept;
}