#pragma once

#include <string>
#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine {
    class GraphicsComponent : public kengine::SerializableComponent<GraphicsComponent>,
                          public putils::Reflectible<GraphicsComponent> {
    public:
        GraphicsComponent(std::string_view appearance = "")
                : appearance(appearance) {}

        const std::string type = pmeta_nameof(GraphicsComponent);
        std::string appearance;
        putils::Point3d size;
        double yaw;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GraphicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&GraphicsComponent::type),
                pmeta_reflectible_attribute(&GraphicsComponent::appearance),
                pmeta_reflectible_attribute(&GraphicsComponent::size),
                pmeta_reflectible_attribute(&GraphicsComponent::yaw)
        );
    };
}
