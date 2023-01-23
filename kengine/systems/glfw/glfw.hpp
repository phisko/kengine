#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_GLFW_EXPORT entt::entity add_glfw(entt::registry & r) noexcept;
}