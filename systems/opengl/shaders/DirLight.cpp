#include "DirLight.hpp"

#include "EntityManager.hpp"
#include "helpers/LightHelper.hpp"

#include "data/LightComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "ShaderHelper.hpp"
#include "QuadSrc.hpp"
#include "ShadowMapShader.hpp"

namespace kengine {
	static bool DEBUG_CSM = false;
}

namespace kengine::Shaders {
	DirLight::DirLight(EntityManager & em, Entity & parent)
		: Program(true, putils_nameof(DirLight)),
		_em(em)
	{
#ifndef KENGINE_NDEBUG
		parent += AdjustableComponent{
			"Render/Lights", {
				{ "Debug CSM", &DEBUG_CSM }
			}
		};
#endif
	}

	void DirLight::init(size_t firstTextureID) {
		initWithShaders<DirLight>(putils::make_vector(
			ShaderDescription{ src::Quad::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::CSM::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::DirLight::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		for (size_t i = 0; i < lengthof(_shadowMap); ++i)
			_shadowMap[i] = _shadowMapTextureID + i;
	}

	void DirLight::run(const Parameters & params) {
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();

		_proj = params.proj;
		_view = params.view;

		_debugCSM = DEBUG_CSM;
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewPort.size);

		for (auto &[e, light] : _em.getEntities<DirLightComponent>()) {
			const putils::Point3f pos = { params.camPos.x, params.camPos.y, params.camPos.z };

			if (light.castShadows) {
				if (e.has<CSMComponent>()) {
					const auto & depthMap = e.get<CSMComponent>();
					ShaderHelper::BindFramebuffer b(depthMap.fbo);
					for (const auto & texture : depthMap.textures) {
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
						glClear(GL_DEPTH_BUFFER_BIT);
					}
				}

				for (const auto & [shadowMapEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowMapShaderComponent>()) {
					auto & shadowMap = static_cast<ShadowMapShader &>(*shader.shader);
					shadowMap.run(e, light, params);
				}
			}

			use();
			setLight(light);

			const auto & depthMap = e.get<CSMComponent>();
			for (size_t i = 0; i < light.cascadeEnds.size(); ++i) {
				glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID + i));
				glBindTexture(GL_TEXTURE_2D, depthMap.textures[i]);
				_lightSpaceMatrix[i] = LightHelper::getCSMLightSpaceMatrix(light, params, i);
			}

			ShaderHelper::shapes::drawQuad();
		}
	}

	void DirLight::setLight(const DirLightComponent & light) {
		_color = light.color;
		_direction = light.direction;

		_ambientStrength = light.ambientStrength;
		_diffuseStrength = light.diffuseStrength;
		_specularStrength = light.specularStrength;
		_pcfSamples = light.shadowPCFSamples;
		_bias = light.shadowMapBias;

		size_t i = 0;
		for (const auto cascadeEnd : light.cascadeEnds) {
			_cascadeEnd[i] = cascadeEnd;
			++i;
		}
		_cascadeCount = (int)light.cascadeEnds.size();
	}
}
