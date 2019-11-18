#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace DepthCube {
		namespace Geom {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::mat4> _shadowMatrices[6];

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_shadowMatrices)
				);
			};
		}

		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::Point3f> _lightPos;
				putils::gl::Uniform<float> _farPlane;

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_lightPos), 
					putils_reflection_attribute_private(&Uniforms::_farPlane)
				);
			};
		}
	}
}