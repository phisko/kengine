#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_INPUT_EXPORT entt::entity add_input(entt::registry & r) noexcept;
}