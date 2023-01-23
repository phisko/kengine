#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_COLLISION_EXPORT entt::entity add_collision(entt::registry & r) noexcept;
}