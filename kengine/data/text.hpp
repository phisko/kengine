#pragma once

#ifndef KENGINE_FONT_PATH_MAX_LENGTH
#define KENGINE_FONT_PATH_MAX_LENGTH 128
#endif

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

// kengine data
#include "kengine/data/on_screen.hpp"

namespace kengine::data {
	struct text {
		static constexpr char string_name[] = "text_string";
		using string = putils::string<KENGINE_FONT_PATH_MAX_LENGTH, string_name>;

		enum class alignment_type {
			left,
			center,
			right
		};

		string font;
		string value;
		size_t font_size;
		putils::normalized_color color;
		alignment_type alignment = alignment_type::center;
	};

	// UI element
	struct text_2d : text, on_screen {};

	// Ground feedback and such
	struct text_3d : text {};
}

#define refltype kengine::data::text
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute(font),
		putils_reflection_attribute(value),
		putils_reflection_attribute(font_size),
		putils_reflection_attribute(color),
		putils_reflection_attribute(alignment)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype

#define refltype kengine::data::text_2d
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::text),
		putils_reflection_type(kengine::data::on_screen)
	);
};
#undef refltype

#define refltype kengine::data::text_3d
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::text)
	);
};
#undef refltype
