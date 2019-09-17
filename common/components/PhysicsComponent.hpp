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

        pmeta_get_class_name(PhysicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&PhysicsComponent::solid),
                pmeta_reflectible_attribute(&PhysicsComponent::kinematic),
                pmeta_reflectible_attribute(&PhysicsComponent::mass),

                pmeta_reflectible_attribute(&PhysicsComponent::movement),
                pmeta_reflectible_attribute(&PhysicsComponent::speed),
                pmeta_reflectible_attribute(&PhysicsComponent::changed)
        );
		pmeta_get_methods();
		pmeta_get_parents();
    };
}
