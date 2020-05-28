#pragma once

#include "Entity.hpp"
#include "Point.hpp"
#include "reflection.hpp"

namespace kengine {
	struct PathfindingComponent {
		Entity::ID environment = Entity::INVALID_ID; // Entity in which we are navigating. Should have a model with a NavMeshComponent
		putils::Point3f destination;
		float searchDistance = 2.f;

		putils_reflection_class_name(PathfindingComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&PathfindingComponent::environment),
			putils_reflection_attribute(&PathfindingComponent::destination),
			putils_reflection_attribute(&PathfindingComponent::searchDistance)
		);
	};
}