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

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_scattering), 
					putils_reflection_attribute_private(&Uniforms::_nbSteps), 
					putils_reflection_attribute_private(&Uniforms::_defaultStepLength), 
					putils_reflection_attribute_private(&Uniforms::_intensity), 
					putils_reflection_attribute_private(&Uniforms::_inverseView), 
					putils_reflection_attribute_private(&Uniforms::_inverseProj), 
					putils_reflection_attribute_private(&Uniforms::_viewPos), 
					putils_reflection_attribute_private(&Uniforms::_screenSize), 
					putils_reflection_attribute_private(&Uniforms::_color)
				);
			};
		}
	}
}
