#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct imgui_scale {
		float scale = 1.f;
	};
}

#define refltype kengine::data::imgui_scale
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scale)
	);
};
#undef refltype