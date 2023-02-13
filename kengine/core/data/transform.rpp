#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::transform
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(bounding_box),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::rect3f)
	);
};
#undef refltype