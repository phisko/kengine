#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace ProjViewModel {
		namespace Vert {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::mat4> _proj;
				putils::gl::Uniform<glm::mat4> _view;
				putils::gl::Uniform<glm::mat4> _model;

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_proj), 
					putils_reflection_attribute_private(&Uniforms::_view), 
					putils_reflection_attribute_private(&Uniforms::_model)
				);
			};
		}
	}
}
