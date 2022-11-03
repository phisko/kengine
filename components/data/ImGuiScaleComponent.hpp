#pragma once

// reflection
#include "reflection.hpp"

namespace kengine {
	struct ImGuiScaleComponent {
		float scale = 1.f;
	};
}

#define refltype kengine::ImGuiScaleComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scale)
	);
};
#undef refltype