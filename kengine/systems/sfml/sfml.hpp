#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_SFML_EXPORT entt::entity add_sfml(entt::registry & r) noexcept;
}