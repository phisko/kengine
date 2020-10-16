#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::opengl::shaders::src {
	namespace DirLight {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::vec3> _viewPos;
				putils::gl::Uniform<putils::Point2f> _screenSize;
				putils::gl::Uniform<putils::NormalizedColor> _color;
				putils::gl::Uniform<putils::Vector3f> _direction;
				putils::gl::Uniform<float> _ambientStrength;
				putils::gl::Uniform<float> _diffuseStrength;
				putils::gl::Uniform<float> _specularStrength;
				putils::gl::Uniform<bool> _debugCSM;
			};
		}

		namespace GetDirection {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::Vector3f> _direction;
			};
		}
	}
}

#define refltype kengine::opengl::shaders::src::DirLight::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_viewPos),
		putils_reflection_attribute_private(_screenSize),
		putils_reflection_attribute_private(_color),
		putils_reflection_attribute_private(_direction),
		putils_reflection_attribute_private(_ambientStrength),
		putils_reflection_attribute_private(_diffuseStrength),
		putils_reflection_attribute_private(_specularStrength),
		putils_reflection_attribute_private(_debugCSM)
	);
};
#undef refltype

#define refltype kengine::opengl::shaders::src::DirLight::GetDirection::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_direction)
	);
};
#undef refltype