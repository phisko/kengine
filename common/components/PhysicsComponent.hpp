#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine {
    class PhysicsComponent : public kengine::SerializableComponent<PhysicsComponent>,
                             public putils::Reflectible<PhysicsComponent> {
    public:
        PhysicsComponent(bool solid = true, bool fixed = false)
                : solid(solid), fixed(false) {}

    public:
        const std::string type = pmeta_nameof(PhysicsComponent);
        bool solid = true;
        bool fixed = false;
        putils::Point3d movement;
        double speed = 1;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(PhysicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&PhysicsComponent::type),
                pmeta_reflectible_attribute(&PhysicsComponent::solid),
                pmeta_reflectible_attribute(&PhysicsComponent::fixed),
                pmeta_reflectible_attribute(&PhysicsComponent::movement),
                pmeta_reflectible_attribute(&PhysicsComponent::speed)
        );
    };
}