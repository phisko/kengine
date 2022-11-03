#pragma once

// reflection
#include "reflection.hpp"

// putils
#include "Rect.hpp"

namespace kengine {
    struct TransformComponent {
        putils::Rect3f boundingBox{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
        float yaw = 0; // Radians
        float pitch = 0; // Radians
		float roll = 0; // Radians
	};
};

#define refltype kengine::TransformComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(boundingBox),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll)
	);
};
#undef refltype
