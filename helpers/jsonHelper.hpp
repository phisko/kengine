#pragma

#include "kengine.hpp"

// nlohmann
#include <nlohmann/json.hpp>

namespace kengine::jsonHelper {
	Entity createEntity(const nlohmann::json & entityJSON) noexcept;
	void loadEntity(const nlohmann::json & entityJSON, Entity & e) noexcept;
	nlohmann::json saveEntity(const Entity & e) noexcept;
}