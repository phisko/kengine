#pragma once

#ifndef KENGINE_GRAPHICS_STRING_MAX_LENGTH
#define KENGINE_GRAPHICS_STRING_MAX_LENGTH 128
#endif

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string, putils::normalized_color]
	struct graphics {
		static constexpr char string_name[] = "graphics_string";
		using string = putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, string_name>;

		string appearance;
		putils::normalized_color color{ 1.f, 1.f, 1.f, 1.f };
	};
}

#include "graphics.rpp"