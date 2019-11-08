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

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_viewPos),
					pmeta_reflectible_attribute_private(&Uniforms::_screenSize),
					pmeta_reflectible_attribute_private(&Uniforms::_color),
					pmeta_reflectible_attribute_private(&Uniforms::_position),
					pmeta_reflectible_attribute_private(&Uniforms::_direction),
					pmeta_reflectible_attribute_private(&Uniforms::_cutOff),
					pmeta_reflectible_attribute_private(&Uniforms::_outerCutOff),
					pmeta_reflectible_attribute_private(&Uniforms::_diffuseStrength),
					pmeta_reflectible_attribute_private(&Uniforms::_specularStrength),
					pmeta_reflectible_attribute_private(&Uniforms::_attenuationConstant),
					pmeta_reflectible_attribute_private(&Uniforms::_attenuationLinear),
					pmeta_reflectible_attribute_private(&Uniforms::_attenuationQuadratic)
				);
			};
		}
	}
}
