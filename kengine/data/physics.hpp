#pragma once

// putils
#include "putils/point.hpp"

namespace kengine::data {
	struct physics {
		float mass = 1.f;

		putils::vec3f movement;
		float yaw = 0.f; // radians to turn in frame
		float pitch = 0.f; // radians to turn in frame
		float roll = 0.f; // radians to turn in frame
		bool changed = false;
	};
}

#define refltype kengine::data::physics
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(mass),
		putils_reflection_attribute(movement),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll),
		putils_reflection_attribute(changed)
	);
};
#undef refltype
