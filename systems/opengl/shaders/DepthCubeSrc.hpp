#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::opengl::shaders::src {
	namespace DepthCube {
		namespace Geom {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::mat4> _shadowMatrices[6];
			};
		}

		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::Point3f> _lightPos;
				putils::gl::Uniform<float> _farPlane;
			};
		}
	}
}

#define refltype kengine::opengl::shaders::src::DepthCube::Geom::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_shadowMatrices)
	);
};
#undef refltype

#define refltype kengine::opengl::shaders::src::DepthCube::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_lightPos), 
		putils_reflection_attribute_private(_farPlane)
	);
};
#undef refltype
