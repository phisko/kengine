#pragma once

// reflection
#include "reflection.hpp"

// putils
#include "Point.hpp"

// kengine
#include "Entity.hpp"

namespace kengine {
	struct PathfindingComponent {
		EntityID environment = INVALID_ID; // Entity in which we are navigating. Should have a model with a NavMeshComponent
		putils::Point3f destination;
		float searchDistance = 2.f;
		float maxSpeed = 1.f;
	};
}

#define refltype kengine::PathfindingComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(environment),
		putils_reflection_attribute(destination),
		putils_reflection_attribute(searchDistance),
		putils_reflection_attribute(maxSpeed)
	);
};
#undef refltype