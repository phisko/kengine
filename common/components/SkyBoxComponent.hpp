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

		putils_reflection_class_name(SkyBoxComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&SkyBoxComponent::right),
			putils_reflection_attribute(&SkyBoxComponent::left),
			putils_reflection_attribute(&SkyBoxComponent::top),
			putils_reflection_attribute(&SkyBoxComponent::bottom),
			putils_reflection_attribute(&SkyBoxComponent::front),
			putils_reflection_attribute(&SkyBoxComponent::back),
			putils_reflection_attribute(&SkyBoxComponent::color)
		);
	};
}
