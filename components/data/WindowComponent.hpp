#pragma once

// reflection
#include "reflection.hpp"

// putils
#include "string.hpp"
#include "Point.hpp"

#ifndef KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH
# define KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH 64
#endif

namespace kengine {
	struct WindowComponent {
		static constexpr char stringName[] = "WindowComponentString";
		using string = putils::string<KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH, stringName>;

		string name;
		putils::Point2ui size = { 1280, 720 };
		bool fullscreen = false;
		string assignedSystem;
	};
}

#define refltype kengine::WindowComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(size),
		putils_reflection_attribute(fullscreen),
		putils_reflection_attribute(assignedSystem)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype