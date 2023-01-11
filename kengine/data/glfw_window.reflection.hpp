#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::glfw_window
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::data::glfw_window_init
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(set_hints),
		putils_reflection_attribute(on_window_created)
	);
};
#undef refltype