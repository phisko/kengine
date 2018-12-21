#pragma once

#ifndef KENGINE_GUI_TEXT_MAX_LENGTH
# define KENGINE_GUI_TEXT_MAX_LENGTH 64
#endif

#ifndef KENGINE_GUI_LIST_MAX_ITEMS
# define KENGINE_GUI_LIST_MAX_ITEMS 64
#endif

#define PUTILS_STRING_LENGTH KENGINE_GUI_TEXT_MAX_LENGTH
#include "string.hpp"
#undef PUTILS_STRING_LENGTH

#define PUTILS_VECTOR_CLASS putils::string<KENGINE_GUI_TEXT_MAX_LENGTH>
#define PUTILS_VECTOR_LENGTH KENGINE_GUI_LIST_MAX_ITEMS
#include "vector.hpp"
#undef PUTILS_VECTOR_CLASS
#undef PUTILS_VECTOR_LENGTH

#include "Point.hpp"
#include "Observable.hpp"

namespace kengine {
    class GUIComponent : public putils::Observable<>, 
                         public putils::Reflectible<GUIComponent> {
    public:
		using string = putils::string<KENGINE_GUI_TEXT_MAX_LENGTH>;
		using item_vector = putils::vector<string, KENGINE_GUI_LIST_MAX_ITEMS>;

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
