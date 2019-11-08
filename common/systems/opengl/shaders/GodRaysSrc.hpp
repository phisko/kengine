#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace GodRays {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<float> _scattering;
				putils::gl::Uniform<float> _nbSteps;
				putils::gl::Uniform<float> _defaultStepLength;
				putils::gl::Uniform<float> _intensity;

				putils::gl::Uniform<glm::mat4> _inverseView;
				putils::gl::Uniform<glm::mat4> _inverseProj;
				putils::gl::Uniform<glm::vec3> _viewPos;
				putils::gl::Uniform<putils::Point2f> _screenSize;

				putils::gl::Uniform<putils::NormalizedColor> _color;

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_scattering), 
					pmeta_reflectible_attribute_private(&Uniforms::_nbSteps), 
					pmeta_reflectible_attribute_private(&Uniforms::_defaultStepLength), 
					pmeta_reflectible_attribute_private(&Uniforms::_intensity), 
					pmeta_reflectible_attribute_private(&Uniforms::_inverseView), 
					pmeta_reflectible_attribute_private(&Uniforms::_inverseProj), 
					pmeta_reflectible_attribute_private(&Uniforms::_viewPos), 
					pmeta_reflectible_attribute_private(&Uniforms::_screenSize), 
					pmeta_reflectible_attribute_private(&Uniforms::_color)
				);
			};
		}
	}
}
