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

		pmeta_get_class_name(WindowComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&WindowComponent::name),
			pmeta_reflectible_attribute(&WindowComponent::size),
			pmeta_reflectible_attribute(&WindowComponent::fullscreen),
			pmeta_reflectible_attribute(&WindowComponent::shutdownOnClose),
			pmeta_reflectible_attribute(&WindowComponent::assignedSystem)
		);
	};
}