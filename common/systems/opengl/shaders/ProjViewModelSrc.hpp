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

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_proj), 
					pmeta_reflectible_attribute_private(&Uniforms::_view), 
					pmeta_reflectible_attribute_private(&Uniforms::_model)
				);
			};
		}
	}
}
