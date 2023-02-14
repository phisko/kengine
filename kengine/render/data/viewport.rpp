#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::viewport
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(bounding_box),
		putils_reflection_attribute(resolution),
		putils_reflection_attribute(z_order),
		putils_reflection_attribute(window),
		putils_reflection_attribute(texture)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::data::on_screen)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::rect2f),
		putils_reflection_type(putils::point2i)
	);
};
#undef refltype