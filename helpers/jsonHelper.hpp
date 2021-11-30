#pragma

#include "kengine.hpp"
#include "json.hpp"

namespace kengine::jsonHelper {
	Entity createEntity(const putils::json & entityJSON) noexcept;
	void loadEntity(const putils::json & entityJSON, Entity & e) noexcept;
	putils::json saveEntity(const Entity & e) noexcept;
}