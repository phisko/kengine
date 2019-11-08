#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace ShadowCube {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap;
				putils::gl::Uniform<putils::Point3f> _position;
				putils::gl::Uniform<glm::vec3> _viewPos;
				putils::gl::Uniform<float> _farPlane;
				putils::gl::Uniform<float> _bias;

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_shadowMap), 
					pmeta_reflectible_attribute_private(&Uniforms::_position), 
					pmeta_reflectible_attribute_private(&Uniforms::_viewPos), 
					pmeta_reflectible_attribute_private(&Uniforms::_farPlane), 
					pmeta_reflectible_attribute_private(&Uniforms::_bias)
				);
			};
		}
	}
}
