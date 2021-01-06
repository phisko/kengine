#include "SpotLight.hpp"

#include "kengine.hpp"

#include "data/TransformComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/lightHelper.hpp"
#include "shaderHelper.hpp"

#include "ShadowMapShader.hpp"

namespace kengine::opengl::shaders {
	void SpotLight::init(size_t firstTextureID) noexcept {
		initWithShaders<SpotLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::ShadowMap::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::SpotLight::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void SpotLight::run(const Parameters & params) noexcept {
		shaderHelper::Enable __c(GL_CULL_FACE);
		shaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewport.size);


		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto [e, light, transform] : entities.with<SpotLightComponent, TransformComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & centre = transform.boundingBox.position;
			setLight(light, centre);

			if (light.castShadows) {
				const auto depthMap = e.tryGet<DepthMapComponent>();
				if (depthMap) {
					shaderHelper::BindFramebuffer b(depthMap->fbo);
					glClear(GL_DEPTH_BUFFER_BIT);
				}

				for (const auto & [shadowMapEntity, shader, shadowMapShader] : entities.with<ShaderComponent, ShadowMapShaderComponent>()) {
					auto & shadowMap = static_cast<ShadowMapShader &>(*shader.shader);
					shadowMap.run(e, light, centre, params);
				}
			}
			use();

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			const auto radius = lightHelper::getRadius(light);
			model = glm::scale(model, { radius, radius, radius });
			_proj = params.proj;
			_view = params.view;
			_model = model;

			const auto centreToCam = putils::Point3f{ params.camPos.x, params.camPos.y, params.camPos.z } - centre;
			if (putils::getLengthSquared(centreToCam) < radius * radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			_lightSpaceMatrix = lightHelper::getLightSpaceMatrix(light, { centre.x, centre.y, centre.z }, params);

			shaderHelper::shapes::drawSphere();
		}

		glCullFace(GL_BACK);
	}

	void SpotLight::setLight(const SpotLightComponent & light, const putils::Point3f & pos) noexcept {
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
