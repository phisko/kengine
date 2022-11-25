#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_COLLISION_EXPORT void add_collision(entt::registry & r) noexcept;
}