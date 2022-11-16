#pragma once

#ifndef KENGINE_FONT_PATH_MAX_LENGTH
# define KENGINE_FONT_PATH_MAX_LENGTH 128
#endif

// reflection
#include "reflection.hpp"

// putils
#include "string.hpp"
#include "Color.hpp"

// kengine data
#include "OnScreenComponent.hpp"

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
	};

	// UI element
	struct TextComponent2D : TextComponent, OnScreenComponent {};

	// Ground feedback and such
	struct TextComponent3D : TextComponent {};
}

#define refltype kengine::TextComponent
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute(font),
		putils_reflection_attribute(text),
		putils_reflection_attribute(fontSize),
		putils_reflection_attribute(color),
		putils_reflection_attribute(alignment)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::NormalizedColor)
	);
};
#undef refltype

#define refltype kengine::TextComponent2D
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::TextComponent),
		putils_reflection_type(kengine::OnScreenComponent)
	);
};
#undef refltype

#define refltype kengine::TextComponent3D
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::TextComponent)
	);
};
#undef refltype
