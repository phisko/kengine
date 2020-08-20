#pragma once

#ifndef KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH 128
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
	};
}

#define refltype kengine::TextureModelComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(file)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype