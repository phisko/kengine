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

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SkyBoxComponent::right),
			pmeta_reflectible_attribute(&SkyBoxComponent::left),
			pmeta_reflectible_attribute(&SkyBoxComponent::top),
			pmeta_reflectible_attribute(&SkyBoxComponent::bottom),
			pmeta_reflectible_attribute(&SkyBoxComponent::front),
			pmeta_reflectible_attribute(&SkyBoxComponent::back),
			pmeta_reflectible_attribute(&SkyBoxComponent::color)
		);

		pmeta_get_class_name(SkyBoxComponent);
	};
}
