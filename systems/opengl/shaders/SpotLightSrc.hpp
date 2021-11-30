#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::opengl::shaders::src {
	namespace SpotLight {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<glm::vec3> _viewPos;
				putils::gl::Uniform<putils::Point2f> _screenSize;
				putils::gl::Uniform<putils::NormalizedColor> _color;
				putils::gl::Uniform<putils::Point3f> _position;
				putils::gl::Uniform<putils::Vector3f> _direction;
				putils::gl::Uniform<float> _cutOff;
				putils::gl::Uniform<float> _outerCutOff;
				putils::gl::Uniform<float> _diffuseStrength;
				putils::gl::Uniform<float> _specularStrength;
				putils::gl::Uniform<float> _attenuationConstant;
				putils::gl::Uniform<float> _attenuationLinear;
				putils::gl::Uniform<float> _attenuationQuadratic;
			};
		}
	}
}

#define refltype kengine::opengl::shaders::src::SpotLight::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_viewPos),
		putils_reflection_attribute_private(_screenSize),
		putils_reflection_attribute_private(_color),
		putils_reflection_attribute_private(_position),
		putils_reflection_attribute_private(_direction),
		putils_reflection_attribute_private(_cutOff),
		putils_reflection_attribute_private(_outerCutOff),
		putils_reflection_attribute_private(_diffuseStrength),
		putils_reflection_attribute_private(_specularStrength),
		putils_reflection_attribute_private(_attenuationConstant),
		putils_reflection_attribute_private(_attenuationLinear),
		putils_reflection_attribute_private(_attenuationQuadratic)
	);
};
#undef refltype
