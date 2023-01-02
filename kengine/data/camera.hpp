#pragma once

// putils
#include "putils/rect.hpp"

namespace kengine::data {
	struct camera {
		putils::rect3f frustum{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
		float pitch = 0; // Radians
		float yaw = 0; // Radians
		float roll = 0; // Radians
		float near_plane = 1.f;
		float far_plane = 1000.f;
	};
};

#define refltype kengine::data::camera
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(frustum),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(roll),
		putils_reflection_attribute(near_plane),
		putils_reflection_attribute(far_plane)
	);
};
#undef refltype
