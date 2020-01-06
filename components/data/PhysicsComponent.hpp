#pragma once

#include "Point.hpp"

namespace kengine {
	struct PhysicsComponent {
		float mass = 1.f;

		putils::Vector3f movement;
		float yaw = 0.f; // radians to turn in frame
		float pitch = 0.f; // radians to turn in frame
		float roll = 0.f; // radians to turn in frame
		float speed = 1.f;
		bool changed = false;

		putils_reflection_class_name(PhysicsComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&PhysicsComponent::mass),

			putils_reflection_attribute(&PhysicsComponent::movement),
			putils_reflection_attribute(&PhysicsComponent::yaw),
			putils_reflection_attribute(&PhysicsComponent::pitch),
			putils_reflection_attribute(&PhysicsComponent::roll),
			putils_reflection_attribute(&PhysicsComponent::speed),
			putils_reflection_attribute(&PhysicsComponent::changed)
		);
	};
}