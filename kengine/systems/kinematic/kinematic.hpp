#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_KINEMATIC_EXPORT entt::entity add_kinematic(entt::registry & r) noexcept;
}