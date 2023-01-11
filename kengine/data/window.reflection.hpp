#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::window
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(size),
		putils_reflection_attribute(fullscreen),
		putils_reflection_attribute(assigned_system)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::point2ui)
	);
};
#undef refltype