#pragma once

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/rect.hpp"

namespace kengine::data {
    struct transform {
        putils::rect3f bounding_box{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
        float yaw = 0; // Radians
        float pitch = 0; // Radians
		float roll = 0; // Radians
	};
};

#define refltype kengine::data::transform
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(bounding_box),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::rect3f)
	);
};
#undef refltype
