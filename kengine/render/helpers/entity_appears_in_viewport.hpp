#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::render {
	KENGINE_RENDER_EXPORT bool entity_appears_in_viewport(const entt::registry & r, entt::entity entity, entt::entity viewport_entity) noexcept;
}