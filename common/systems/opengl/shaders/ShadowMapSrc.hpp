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

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_shadowMap),
					pmeta_reflectible_attribute_private(&Uniforms::_lightSpaceMatrix),
					pmeta_reflectible_attribute_private(&Uniforms::_bias),
					pmeta_reflectible_attribute_private(&Uniforms::_pcfSamples)
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

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_shadowMap), 
					pmeta_reflectible_attribute_private(&Uniforms::_lightSpaceMatrix), 
					pmeta_reflectible_attribute_private(&Uniforms::_cascadeEnd), 
					pmeta_reflectible_attribute_private(&Uniforms::_bias), 
					pmeta_reflectible_attribute_private(&Uniforms::_pcfSamples), 
					pmeta_reflectible_attribute_private(&Uniforms::_proj), 
					pmeta_reflectible_attribute_private(&Uniforms::_view)
				);
			};
		}
	}
}
