#pragma once

#include <string>
#include "SerializableComponent.hpp"

namespace kengine
{
    class GUIComponent : public kengine::SerializableComponent<GUIComponent>,
                         public putils::Reflectible<GUIComponent>
    {
    public:
        GUIComponent(std::string_view text = "", std::size_t textSize = 18, std::string_view font = "")
                : text(text), textSize(textSize), font(font)
        {}

        const std::string type = pmeta_nameof(GUIComponent);
        std::string text;
        double textSize;
        std::string font;

        /*
         * Reflectible
         */

    public:
        static const auto get_class_name() { return pmeta_nameof(GUIComponent); }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&GUIComponent::type),
                    pmeta_reflectible_attribute(&GUIComponent::text),
                    pmeta_reflectible_attribute(&GUIComponent::textSize),
                    pmeta_reflectible_attribute(&GUIComponent::font)
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
