#pragma once

#include <string>
#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine {
    class GUIComponent : public kengine::SerializableComponent<GUIComponent>,
                         public putils::Reflectible<GUIComponent> {
    public:
        const std::string type = pmeta_nameof(GUIComponent);
		std::function<void()> onClick = nullptr;
        std::string text;
        putils::Rect3d boundingBox;

		enum Type {
			Text,
			Button,
			List
		};

		int guiType;

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GUIComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&GUIComponent::type),
                pmeta_reflectible_attribute(&GUIComponent::onClick),
                pmeta_reflectible_attribute(&GUIComponent::text),
                pmeta_reflectible_attribute(&GUIComponent::boundingBox),
                pmeta_reflectible_attribute(&GUIComponent::guiType)
        );
        pmeta_get_methods();
        pmeta_get_parents();
    };
}
