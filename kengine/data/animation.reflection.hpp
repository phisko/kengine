#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::animation
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(current_anim),
		putils_reflection_attribute(current_time),
		putils_reflection_attribute(speed),
		putils_reflection_attribute(loop),
		putils_reflection_attribute(position_mover_behavior),
		putils_reflection_attribute(rotation_mover_behavior),
		putils_reflection_attribute(scale_mover_behavior)
	);
};
#undef refltype