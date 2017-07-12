#pragma once

#include <string>
#include "SerializableComponent.hpp"

namespace kengine
{
    class MetaComponent : public kengine::SerializableComponent<MetaComponent>,
                          public putils::Reflectible<MetaComponent>
    {
    public:
        MetaComponent(std::string_view appearance = "")
                : appearance(appearance) {}

        const std::string type = "meta";
        std::string appearance;

        /*
         * Reflectible
         */

    public:
        static const auto get_class_name() { return "MetaComponent"; }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    "type", &MetaComponent::type,
                    "appearance", &MetaComponent::appearance
            );
            return table;
        }

        static const auto &get_methods()
        {
            static const auto table = pmeta::make_table();
            return table;
        }

        static const auto &get_parents()
        {
            static const auto table = pmeta::make_table();
            return table;
        }
    };
}
