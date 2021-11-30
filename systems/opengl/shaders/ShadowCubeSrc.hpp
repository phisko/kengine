#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::opengl::shaders::src {
	namespace ShadowCube {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap;
				putils::gl::Uniform<putils::Point3f> _position;
				putils::gl::Uniform<glm::vec3> _viewPos;
				putils::gl::Uniform<float> _farPlane;
				putils::gl::Uniform<float> _bias;
			};
		}
	}
}

#define refltype kengine::opengl::shaders::src::ShadowCube::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_shadowMap), 
		putils_reflection_attribute_private(_position), 
		putils_reflection_attribute_private(_viewPos), 
		putils_reflection_attribute_private(_farPlane), 
		putils_reflection_attribute_private(_bias)
	);
};
#undef refltype
