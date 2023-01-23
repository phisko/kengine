#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_BULLET_EXPORT entt::entity add_bullet(entt::registry & r) noexcept;
}