#pragma once

#include "Point.hpp"

namespace kengine {
	struct PhysicsComponent {
		float mass = 1.f;

		putils::Vector3f movement;
		float yaw = 0.f; // radians to turn in frame
		float pitch = 0.f; // radians to turn in frame
		float roll = 0.f; // radians to turn in frame
		bool changed = false;
	};
}

#define refltype kengine::PhysicsComponent
putils_reflection_info{
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
