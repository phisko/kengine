#pragma once

#include "reflection.hpp"

#ifndef KENGINE_FONT_PATH_MAX_LENGTH
# define KENGINE_FONT_PATH_MAX_LENGTH 128
#endif

#include "string.hpp"
#include "Color.hpp"

namespace kengine {
	struct TextComponent {
		static constexpr char stringName[] = "TextComponentString";
		using string = putils::string<KENGINE_FONT_PATH_MAX_LENGTH, stringName>;

		enum class Alignment {
			Left,
			Center,
			Right
		};

		string font;
		string text;
		size_t fontSize;
		putils::NormalizedColor color;
		Alignment alignment = Alignment::Center;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&TextComponent::font),
			pmeta_reflectible_attribute(&TextComponent::text),
			pmeta_reflectible_attribute(&TextComponent::fontSize),
			pmeta_reflectible_attribute(&TextComponent::color),
			pmeta_reflectible_attribute(&TextComponent::alignment)
		);
	};

	struct TextComponent2D : TextComponent { // UI element
		pmeta_get_class_name(TextComponent2D);
	};

	struct TextComponent3D : TextComponent { // Ground feedback and such
		pmeta_get_class_name(TextComponent3D);
	};
}
