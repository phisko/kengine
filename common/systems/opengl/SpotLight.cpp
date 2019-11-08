#include "SpotLight.hpp"

#include "EntityManager.hpp"

#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ShaderComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

namespace kengine::Shaders {
	void SpotLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SpotLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::ShadowMap::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::SpotLight::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void SpotLight::run(const Parameters & params) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderHelper::Enable __c(GL_CULL_FACE);
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewPort.size);


		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto &[e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & centre = transform.boundingBox.position;
			setLight(light, centre);

			if (light.castShadows)
				for (const auto & [shadowMapEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowMapShaderComponent>()) {
					auto & shadowMap = static_cast<ShadowMapShader &>(*shader.shader);
					shadowMap.run(e, light, centre, params);
				}

			use();

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			const auto radius = LightHelper::getRadius(light);
			model = glm::scale(model, { radius, radius, radius });
			_proj = params.proj;
			_view = params.view;
			_model = model;

			if (centre.getDistanceTo(putils::Point3f{ params.camPos.x, params.camPos.y, params.camPos.z }) < radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			_lightSpaceMatrix = LightHelper::getLightSpaceMatrix(light, { centre.x, centre.y, centre.z }, params);

			ShaderHelper::shapes::drawSphere();
		}

		glCullFace(GL_BACK);
	}

	void SpotLight::setLight(const SpotLightComponent & light, const putils::Point3f & pos) {
		_color = light.color;
		_position = pos;
		_direction = light.direction;

		_cutOff = light.cutOff;
		_outerCutOff = light.outerCutOff;

		_diffuseStrength = light.diffuseStrength;
		_specularStrength = light.specularStrength;

		_attenuationConstant = light.constant;
		_attenuationLinear = light.linear;
		_attenuationQuadratic = light.quadratic;

		_pcfSamples = light.shadowPCFSamples;
		_bias = light.shadowMapBias;
	}
}
