#pragma once

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/point.hpp"

namespace kengine::data {
	struct pathfinding {
		entt::entity environment = entt::null; // Entity in which we are navigating. Should have a model with a data::nav_mesh
		putils::point3f destination;
		float search_distance = 2.f;
		float max_speed = 1.f;
	};
}

#define refltype kengine::data::pathfinding
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(environment),
		putils_reflection_attribute(destination),
		putils_reflection_attribute(search_distance),
		putils_reflection_attribute(max_speed)
	);
};
#undef refltype