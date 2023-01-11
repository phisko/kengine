#pragma once

#ifndef KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME
#define KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME 128
#endif

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string, putils::normalized_color]
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

#include "sky_box.reflection.hpp"