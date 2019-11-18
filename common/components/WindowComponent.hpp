#pragma once

#include "reflection.hpp"
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
		bool shutdownOnClose = true;
		string assignedSystem;

		putils_reflection_class_name(WindowComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&WindowComponent::name),
			putils_reflection_attribute(&WindowComponent::size),
			putils_reflection_attribute(&WindowComponent::fullscreen),
			putils_reflection_attribute(&WindowComponent::shutdownOnClose),
			putils_reflection_attribute(&WindowComponent::assignedSystem)
		);
	};
}