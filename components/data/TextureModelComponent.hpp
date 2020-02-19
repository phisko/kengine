#pragma once

#ifndef KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH 64
#endif

#include "opengl/RAII.hpp"
#include "reflection.hpp"
#include "string.hpp"

namespace kengine {
	struct TextureModelComponent {
		static constexpr char stringName[] = "TextureModelComponentString";
		using string = putils::string<KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH, stringName>;
		string file;

		putils::gl::Texture texture;

		putils_reflection_class_name(TextureModelComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&TextureModelComponent::file)
		);
		putils_reflection_used_types(
			putils_reflection_type(string)
		);
	};
}