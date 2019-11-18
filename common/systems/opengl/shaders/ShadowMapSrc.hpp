#pragma once

#include "opengl/Uniform.hpp"
#include "components/LightComponent.hpp"

namespace kengine::Shaders::src {
	namespace ShadowMap {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap;
				putils::gl::Uniform<glm::mat4> _lightSpaceMatrix;
				putils::gl::Uniform<float> _bias;
				putils::gl::Uniform<int> _pcfSamples;

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_shadowMap),
					putils_reflection_attribute_private(&Uniforms::_lightSpaceMatrix),
					putils_reflection_attribute_private(&Uniforms::_bias),
					putils_reflection_attribute_private(&Uniforms::_pcfSamples)
				);
			};
		}
	}

	namespace CSM {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<size_t> _shadowMap[KENGINE_CSM_COUNT];
				putils::gl::Uniform<glm::mat4> _lightSpaceMatrix[KENGINE_CSM_COUNT];
				putils::gl::Uniform<float> _cascadeEnd[KENGINE_CSM_COUNT];
				putils::gl::Uniform<float> _bias;
				putils::gl::Uniform<int> _pcfSamples;

				putils::gl::Uniform<glm::mat4> _proj;
				putils::gl::Uniform<glm::mat4> _view;

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_shadowMap), 
					putils_reflection_attribute_private(&Uniforms::_lightSpaceMatrix), 
					putils_reflection_attribute_private(&Uniforms::_cascadeEnd), 
					putils_reflection_attribute_private(&Uniforms::_bias), 
					putils_reflection_attribute_private(&Uniforms::_pcfSamples), 
					putils_reflection_attribute_private(&Uniforms::_proj), 
					putils_reflection_attribute_private(&Uniforms::_view)
				);
			};
		}
	}
}
