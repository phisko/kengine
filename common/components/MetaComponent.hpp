#pragma once

#include <string>
#include "SerializableComponent.hpp"

namespace kengine {
    class MetaComponent : public kengine::SerializableComponent<MetaComponent>,
                          public putils::Reflectible<MetaComponent> {
    public:
        MetaComponent(std::string_view appearance = "")
                : appearance(appearance) {}

        const std::string type = pmeta_nameof(MetaComponent);
        std::string appearance;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(MetaComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&MetaComponent::type),
                pmeta_reflectible_attribute(&MetaComponent::appearance)
        );
    };
}
