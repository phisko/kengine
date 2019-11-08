#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
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

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_viewPos), 
					pmeta_reflectible_attribute_private(&Uniforms::_screenSize), 
					pmeta_reflectible_attribute_private(&Uniforms::_color), 
					pmeta_reflectible_attribute_private(&Uniforms::_direction), 
					pmeta_reflectible_attribute_private(&Uniforms::_ambientStrength), 
					pmeta_reflectible_attribute_private(&Uniforms::_diffuseStrength), 
					pmeta_reflectible_attribute_private(&Uniforms::_specularStrength), 
					pmeta_reflectible_attribute_private(&Uniforms::_debugCSM)
				);
			};
		}

		namespace GetDirection {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::Vector3f> _direction;

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_direction)
				);
			};
		}
	}
}