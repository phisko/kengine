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

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_shadowMap), 
					putils_reflection_attribute_private(&Uniforms::_position), 
					putils_reflection_attribute_private(&Uniforms::_viewPos), 
					putils_reflection_attribute_private(&Uniforms::_farPlane), 
					putils_reflection_attribute_private(&Uniforms::_bias)
				);
			};
		}
	}
}
