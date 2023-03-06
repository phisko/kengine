#pragma once

#ifndef KENGINE_FONT_PATH_MAX_LENGTH
#define KENGINE_FONT_PATH_MAX_LENGTH 128
#endif

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/string.hpp"
#include "putils/color.hpp"

// kengine
#include "kengine/render/data/on_screen.hpp"

namespace kengine::render {
	//! kengine registration off
	//! putils reflect all
	//! used_types: [refltype::string, putils::normalized_color]
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
	//! putils reflect all
	//! parents: [kengine::render::text, kengine::render::on_screen]
	struct text_2d : text, on_screen {};

	// Ground feedback and such
	//! putils reflect all
	//! parents: [kengine::render::text]
	struct text_3d : text {};
}

#include "text.rpp"