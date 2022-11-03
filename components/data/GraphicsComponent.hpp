#pragma once

#ifndef KENGINE_GRAPHICS_STRING_MAX_LENGTH
# define KENGINE_GRAPHICS_STRING_MAX_LENGTH 128
#endif

// putils
#include "string.hpp"
#include "Color.hpp"

namespace kengine {
    struct GraphicsComponent {
		static constexpr char stringName[] = "GraphicsComponentString";
		using string = putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, stringName>;

		string appearance;
		putils::NormalizedColor color{ 1.f, 1.f, 1.f, 1.f };
	};
}

#define refltype kengine::GraphicsComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(appearance),
		putils_reflection_attribute(color)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype
