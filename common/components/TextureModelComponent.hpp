#pragma once

#ifndef KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH 64
#endif

#include <gl/glew.h>
#include <GL/GL.h>

#include "reflection.hpp"
#include "string.hpp"

namespace kengine {
	struct TextureModelComponent {
		static constexpr char stringName[] = "TextureModelComponentString";
		putils::string<KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH, stringName> file;

		GLuint texture = -1;

		putils_reflection_class_name(TextureModelComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&TextureModelComponent::file)
		);
	};
}