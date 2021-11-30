#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::opengl::shaders::src {
	namespace ProjViewModel {
		namespace Vert {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::mat4> _proj;
				putils::gl::Uniform<glm::mat4> _view;
				putils::gl::Uniform<glm::mat4> _model;
			};
		}
	}
}

#define refltype kengine::opengl::shaders::src::ProjViewModel::Vert::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_proj), 
		putils_reflection_attribute_private(_view), 
		putils_reflection_attribute_private(_model)
	);
};
#undef refltype
