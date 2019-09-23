#pragma once

#ifndef KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH 64
#endif

#include <gl/glew.h>
#include <GL/GL.h>

#include "not_serializable.hpp"

#include "reflection/Reflectible.hpp"
#include "string.hpp"

namespace kengine {
	struct TextureModelComponent : not_serializable {
		static constexpr char stringName[] = "TextureModelComponentString";
		putils::string<KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH, stringName> file;

		GLuint texture;

		pmeta_get_class_name(TextureModelComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&TextureModelComponent::file)
		);
	};
}