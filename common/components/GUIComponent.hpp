#pragma once

#include <string>
#include "Point.hpp"
#include "Observable.hpp"

namespace kengine {
    class GUIComponent : public putils::Observable<>, 
                         public putils::Reflectible<GUIComponent> {
    public:
		GUIComponent() = default;
		GUIComponent(const std::string & text, const putils::Rect3f & boundingBox)
			: text(text), boundingBox(boundingBox), guiType(Text) {}

		GUIComponent(const std::string & text, const putils::Rect3f & boundingBox, const std::function<void()> & onClick)
			: text(text), boundingBox(boundingBox), onClick(onClick), guiType(Button) {}

        const std::string type = pmeta_nameof(GUIComponent);
        std::function<void()> onClick = nullptr;
        std::string text;
        putils::Rect3f boundingBox;

		// ProgressBar
		struct {
			unsigned int min;
			unsigned int max;
			unsigned int value;
		} progressBar;

		struct {
			std::vector<std::string> items;
			std::function<void(const std::string &)> onItemClick;
		} list;

        enum Type {
            Text,
            Button,
			ProgressBar,
			List
        };

		int guiType = Type::Text;

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
        pmeta_get_methods(
			pmeta_reflectible_attribute(&GUIComponent::changed)
		);
        pmeta_get_parents();
    };
}
