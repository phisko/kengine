#pragma once

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/string.hpp"
#include "putils/point.hpp"

#ifndef KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH
#define KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH 64
#endif

namespace kengine::render {
	//! putils reflect all
	//! used_types: [refltype::string, putils::point2ui]
	struct window {
		static constexpr char string_name[] = "window_string";
		using string = putils::string<KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH, string_name>;

		string name;
		putils::point2ui size = { 1280, 720 };
		bool fullscreen = false;
		string assigned_system;
	};
}

#include "window.rpp"