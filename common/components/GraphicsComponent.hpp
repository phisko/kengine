#pragma once

#include <string>
#include "SerializableComponent.hpp"

namespace kengine {
    class GraphicsComponent : public kengine::SerializableComponent<GraphicsComponent>,
                          public putils::Reflectible<GraphicsComponent> {
    public:
        GraphicsComponent(std::string_view appearance = "")
                : appearance(appearance) {}

        const std::string type = pmeta_nameof(GraphicsComponent);
        std::string appearance;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GraphicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&GraphicsComponent::type),
                pmeta_reflectible_attribute(&GraphicsComponent::appearance)
        );
    };
}
