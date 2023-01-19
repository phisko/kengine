#pragma once

#ifndef KENGINE_COLLISION_FUNCTION_SIZE
#define KENGINE_COLLISION_FUNCTION_SIZE 64
#endif

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/function.hpp"

namespace kengine::data {
	//! putils reflect all
	struct collision {
		using function = putils::function<void(entt::entity, entt::entity), KENGINE_COLLISION_FUNCTION_SIZE>;
		function on_collide = nullptr;
	};
}

#include "collision.rpp"