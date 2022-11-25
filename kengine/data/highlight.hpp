#pragma once

// putils
#include "putils/point.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	struct highlight {
		putils::normalized_color color;
		float intensity = 1.f;
	};
}

#define refltype kengine::data::highlight
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(color),
		putils_reflection_attribute(intensity)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype