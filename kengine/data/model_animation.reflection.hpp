#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::model_animation
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(animations),
		putils_reflection_attribute(get_animation_movement_until_time),
		putils_reflection_attribute(get_animation_rotation_until_time),
		putils_reflection_attribute(get_animation_scaling_until_time)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::anim)
	);
};
#undef refltype

#define refltype kengine::data::model_animation::anim
putils_reflection_info {
	putils_reflection_custom_class_name(model_animation_anim);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(total_time),
		putils_reflection_attribute(ticks_per_second)
	);
};
#undef refltype