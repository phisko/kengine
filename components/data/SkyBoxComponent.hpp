#pragma once

#ifndef KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME
# define KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME 128
#endif

#include "string.hpp"
#include "Color.hpp"

namespace kengine {
	struct SkyBoxComponent {
		static constexpr char stringName[] = "SkyBoxComponentString";
		using string = putils::string<KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME, stringName>;

		string right;
		string left;
		string top;
		string bottom;
		string front;
		string back;

		putils::NormalizedColor color;
	};
}

#define refltype kengine::SkyBoxComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(right),
		putils_reflection_attribute(left),
		putils_reflection_attribute(top),
		putils_reflection_attribute(bottom),
		putils_reflection_attribute(front),
		putils_reflection_attribute(back),
		putils_reflection_attribute(color)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype
