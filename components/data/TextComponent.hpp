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

		putils_reflection_attributes(
			putils_reflection_attribute(&TextComponent::font),
			putils_reflection_attribute(&TextComponent::text),
			putils_reflection_attribute(&TextComponent::fontSize),
			putils_reflection_attribute(&TextComponent::color),
			putils_reflection_attribute(&TextComponent::alignment)
		);
	};

	struct TextComponent2D : TextComponent { // UI element
		putils_reflection_class_name(TextComponent2D);
	};

	struct TextComponent3D : TextComponent { // Ground feedback and such
		putils_reflection_class_name(TextComponent3D);
	};
}
