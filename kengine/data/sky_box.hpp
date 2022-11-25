#pragma once

#ifndef KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME
# define KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME 128
#endif

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	struct sky_box {
		static constexpr char string_name[] = "sky_box_string";
		using string = putils::string<KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME, string_name>;

		string right;
		string left;
		string top;
		string bottom;
		string front;
		string back;

		putils::normalized_color color;
	};
}

#define refltype kengine::data::sky_box
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(right),
		putils_reflection_attribute(left),
		putils_reflection_attribute(top),
		putils_reflection_attribute(bottom),
		putils_reflection_attribute(front),
		putils_reflection_attribute(back),
		putils_reflection_attribute(color)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype
