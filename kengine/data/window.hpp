#pragma once

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/string.hpp"
#include "putils/point.hpp"

#ifndef KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH
#define KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH 64
#endif

namespace kengine::data {
	struct window {
		static constexpr char string_name[] = "window_string";
		using string = putils::string<KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH, string_name>;

		string name;
		putils::point2ui size = { 1280, 720 };
		bool fullscreen = false;
		string assigned_system;
	};
}

#define refltype kengine::data::window
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(size),
		putils_reflection_attribute(fullscreen),
		putils_reflection_attribute(assigned_system)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype