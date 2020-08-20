#pragma once

#include "Point.hpp"

namespace kengine {
    struct CameraComponent {
        putils::Rect3f frustum{ {}, { 1.f, 1.f, 1.f } };
        float pitch = 0; // Radians
        float yaw = 0; // Radians
		float roll = 0; // Radians
	};
};

#define refltype kengine::CameraComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(frustum),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(roll)
	);
};
#undef refltype
