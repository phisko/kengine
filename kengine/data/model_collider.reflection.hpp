#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::model_collider
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(colliders)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::collider)
	);
};
#undef refltype

#define refltype kengine::data::model_collider::collider
putils_reflection_info {
	putils_reflection_custom_class_name(model_collider_collider);
	putils_reflection_attributes(
		putils_reflection_attribute(shape),
		putils_reflection_attribute(bone_name),
		putils_reflection_attribute(transform)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype