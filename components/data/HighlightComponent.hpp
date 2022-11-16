#pragma once

// putils
#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	struct HighlightComponent {
		putils::NormalizedColor color;
		float intensity = 1.f;
	};
}

#define refltype kengine::HighlightComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(color),
		putils_reflection_attribute(intensity)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::NormalizedColor)
	);
};
#undef refltype