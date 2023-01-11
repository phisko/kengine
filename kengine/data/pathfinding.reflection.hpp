#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::pathfinding
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(environment),
		putils_reflection_attribute(destination),
		putils_reflection_attribute(search_distance),
		putils_reflection_attribute(max_speed)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point3f)
	);
};
#undef refltype