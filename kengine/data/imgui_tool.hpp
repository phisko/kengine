#pragma once

#ifndef KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH
# define KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH 64
#endif

// putils
#include "putils/string.hpp"

namespace kengine::data {
	struct imgui_tool {
		bool enabled = false;
	};
}

#define refltype kengine::data::imgui_tool
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(enabled)
	);
};
#undef refltype
