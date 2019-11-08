#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace DepthCube {
		namespace Geom {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::mat4> _shadowMatrices[6];

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_shadowMatrices)
				);
			};
		}

		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::Point3f> _lightPos;
				putils::gl::Uniform<float> _farPlane;

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_lightPos), 
					pmeta_reflectible_attribute_private(&Uniforms::_farPlane)
				);
			};
		}
	}
}