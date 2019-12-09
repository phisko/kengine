#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
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

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_viewPos),
					putils_reflection_attribute_private(&Uniforms::_screenSize),
					putils_reflection_attribute_private(&Uniforms::_color),
					putils_reflection_attribute_private(&Uniforms::_position),
					putils_reflection_attribute_private(&Uniforms::_direction),
					putils_reflection_attribute_private(&Uniforms::_cutOff),
					putils_reflection_attribute_private(&Uniforms::_outerCutOff),
					putils_reflection_attribute_private(&Uniforms::_diffuseStrength),
					putils_reflection_attribute_private(&Uniforms::_specularStrength),
					putils_reflection_attribute_private(&Uniforms::_attenuationConstant),
					putils_reflection_attribute_private(&Uniforms::_attenuationLinear),
					putils_reflection_attribute_private(&Uniforms::_attenuationQuadratic)
				);
			};
		}
	}
}
