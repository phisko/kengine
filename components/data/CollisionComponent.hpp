#pragma once

#ifndef KENGINE_COLLISION_FUNCTION_SIZE
# define KENGINE_COLLISION_FUNCTION_SIZE 64
#endif

// reflection
#include "reflection.hpp"

// putils
#include "function.hpp"

namespace kengine {
	class Entity;

	struct CollisionComponent {
		using function = putils::function<void(Entity &, Entity &), KENGINE_COLLISION_FUNCTION_SIZE>;
		function onCollide = nullptr;
	};
}

#define refltype kengine::CollisionComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(onCollide)
	);
};
#undef refltype
