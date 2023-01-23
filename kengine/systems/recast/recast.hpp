#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_RECAST_EXPORT entt::entity add_recast(entt::registry & r) noexcept;
}