#pragma once

#ifndef KENGINE_GRAPHICS_STRING_MAX_LENGTH
#define KENGINE_GRAPHICS_STRING_MAX_LENGTH 128
#endif

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	struct graphics {
		static constexpr char string_name[] = "graphics_string";
		using string = putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, string_name>;

		string appearance;
		putils::normalized_color color{ 1.f, 1.f, 1.f, 1.f };
	};
}

#define refltype kengine::data::graphics
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(appearance),
		putils_reflection_attribute(color)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype
