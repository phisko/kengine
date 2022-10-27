#pragma once

#include "Rect.hpp"

namespace kengine {
    struct CameraComponent {
        putils::Rect3f frustum{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
        float pitch = 0; // Radians
        float yaw = 0; // Radians
        float roll = 0; // Radians
        float nearPlane = 1.f;
        float farPlane = 1000.f;
	};
};

#define refltype kengine::CameraComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(frustum),
		putils_reflection_attribute(pitch),
        putils_reflection_attribute(yaw),
        putils_reflection_attribute(roll),
        putils_reflection_attribute(nearPlane),
        putils_reflection_attribute(farPlane)
	);
};
#undef refltype
