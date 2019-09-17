#pragma once

#ifndef KENGINE_GUI_TEXT_MAX_LENGTH
# define KENGINE_GUI_TEXT_MAX_LENGTH 64
#endif

#ifndef KENGINE_GUI_LIST_MAX_ITEMS
# define KENGINE_GUI_LIST_MAX_ITEMS 64
#endif

#include "string.hpp"
#include "vector.hpp"
#include "Point.hpp"
#include "Observable.hpp"

namespace kengine {
    class GUIComponent : public putils::Observable<>, 
                         public kengine::not_serializable {
    public:
	    	static constexpr char stringName[] = "GUIComponentString";
		using string = putils::string<KENGINE_GUI_TEXT_MAX_LENGTH, stringName>;
		static constexpr char vectorName[] = "GUIComponentVector";
		using item_vector = putils::vector<string, KENGINE_GUI_LIST_MAX_ITEMS, vectorName>;

    public:
		GUIComponent() = default;
		GUIComponent(const char * text, const putils::Rect3f & boundingBox)
			: text(text), boundingBox(boundingBox), guiType(Text) {}

		GUIComponent(const char * text, const putils::Rect3f & boundingBox, const std::function<void()> & onClick)
			: text(text), boundingBox(boundingBox), onClick(onClick), guiType(Button) {}

        std::function<void()> onClick = nullptr;
        string text;
        putils::Rect3f boundingBox;

		// ProgressBar
		struct {
			unsigned int min;
			unsigned int max;
			unsigned int value;
		} progressBar;

		struct {
			item_vector items;
			std::function<void(const char *)> onItemClick;
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
