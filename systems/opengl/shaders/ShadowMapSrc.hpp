#pragma once

#include "opengl/Uniform.hpp"
#include "data/LightComponent.hpp"

namespace kengine::Shaders::src {
	namespace ShadowMap {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap;
				putils::gl::Uniform<glm::mat4> _lightSpaceMatrix;
				putils::gl::Uniform<float> _bias;
				putils::gl::Uniform<int> _pcfSamples;
			};
		}
	}

	namespace CSM {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap[KENGINE_MAX_CSM_COUNT];
				putils::gl::Uniform<glm::mat4> _lightSpaceMatrix[KENGINE_MAX_CSM_COUNT];
				putils::gl::Uniform<float> _cascadeEnd[KENGINE_MAX_CSM_COUNT];
				putils::gl::Uniform<int> _cascadeCount;
				putils::gl::Uniform<float> _bias;
				putils::gl::Uniform<int> _pcfSamples;

				putils::gl::Uniform<glm::mat4> _proj;
				putils::gl::Uniform<glm::mat4> _view;
			};
		}
	}
}

#define refltype kengine::Shaders::src::ShadowMap::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_shadowMap),
		putils_reflection_attribute_private(_lightSpaceMatrix),
		putils_reflection_attribute_private(_bias),
		putils_reflection_attribute_private(_pcfSamples)
	);
};
#undef refltype

#define refltype kengine::Shaders::src::CSM::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_shadowMap), 
		putils_reflection_attribute_private(_lightSpaceMatrix), 
		putils_reflection_attribute_private(_cascadeEnd), 
		putils_reflection_attribute_private(_cascadeCount), 
		putils_reflection_attribute_private(_bias), 
		putils_reflection_attribute_private(_pcfSamples), 
		putils_reflection_attribute_private(_proj), 
		putils_reflection_attribute_private(_view)
	);
};
#undef refltype
