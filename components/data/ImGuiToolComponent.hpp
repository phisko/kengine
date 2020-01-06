#pragma once

#ifndef KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH
# define KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH 64
#endif

#include "string.hpp"

namespace kengine {
	struct ImGuiToolComponent {
		static constexpr char stringName[] = "ImGuiToolComponentString";
		bool enabled;

		putils_reflection_class_name(ImGuiToolComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ImGuiToolComponent::enabled)
		);
	};
}
