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
			};
		}
	}
}

#define refltype kengine::Shaders::src::GodRays::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_scattering), 
		putils_reflection_attribute_private(_nbSteps), 
		putils_reflection_attribute_private(_defaultStepLength), 
		putils_reflection_attribute_private(_intensity), 
		putils_reflection_attribute_private(_inverseView), 
		putils_reflection_attribute_private(_inverseProj), 
		putils_reflection_attribute_private(_viewPos), 
		putils_reflection_attribute_private(_screenSize), 
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype
