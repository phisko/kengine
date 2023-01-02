#pragma once

#ifndef KENGINE_COLLISION_FUNCTION_SIZE
#define KENGINE_COLLISION_FUNCTION_SIZE 64
#endif

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/function.hpp"

namespace kengine::data {
	struct collision {
		using function = putils::function<void(entt::entity, entt::entity), KENGINE_COLLISION_FUNCTION_SIZE>;
		function on_collide = nullptr;
	};
}

#define refltype kengine::data::collision
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(on_collide)
	);
};
#undef refltype
