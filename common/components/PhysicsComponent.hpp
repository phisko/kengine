#pragma once

#include "Point.hpp"

namespace kengine {
    struct PhysicsComponent {
        bool solid = true;
		bool kinematic = false;
		float mass = 1.f;

        putils::Vector3f movement;
        float speed = 1;
		bool changed = false;

        putils_reflection_class_name(PhysicsComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&PhysicsComponent::solid),
                putils_reflection_attribute(&PhysicsComponent::kinematic),
                putils_reflection_attribute(&PhysicsComponent::mass),

                putils_reflection_attribute(&PhysicsComponent::movement),
                putils_reflection_attribute(&PhysicsComponent::speed),
                putils_reflection_attribute(&PhysicsComponent::changed)
        );
    };
}
