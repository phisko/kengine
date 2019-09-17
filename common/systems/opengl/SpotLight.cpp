#include "SpotLight.hpp"

#include "ShadowMap.hpp"
#include "EntityManager.hpp"
#include "shaders/shaders.hpp"

#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

namespace kengine {
	extern float SHADOW_MAP_MIN_BIAS;
	extern float SHADOW_MAP_MAX_BIAS;
}

namespace kengine::Shaders {
	void SpotLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SpotLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::ShadowMap::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::SpotLight::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(this->shadowMap, _shadowMapTextureID);
	}

	void SpotLight::run(const Parameters & params) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderHelper::Enable __c(GL_CULL_FACE);
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(viewPos, params.camPos);
		putils::gl::setUniform(screenSize, params.screenSize);

		putils::gl::setUniform(shadow_map_min_bias, SHADOW_MAP_MIN_BIAS);
		putils::gl::setUniform(shadow_map_max_bias, SHADOW_MAP_MAX_BIAS);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto &[e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & centre = transform.boundingBox.position;

			for (const auto & [shadowMapEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowMapShaderComponent>()) {
				auto & shadowMap = static_cast<ShadowMapShader &>(*shader.shader);
				shadowMap.run(e, light, centre, (size_t)params.screenSize.x, (size_t)params.screenSize.y);
			}

			use();

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			const auto radius = LightHelper::getRadius(light);
			model = glm::scale(model, { radius, radius, radius });
			putils::gl::setUniform(this->proj, params.proj);
			putils::gl::setUniform(this->view, params.view);
			putils::gl::setUniform(this->model, model);

			if (centre.distanceTo(putils::Point3f{ params.camPos.x, params.camPos.y, params.camPos.z }) < radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			setLight(light, centre);

			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, { centre.x, centre.y, centre.z }, (size_t)params.screenSize.x, (size_t)params.screenSize.y));

			ShaderHelper::shapes::drawSphere();
		}

		glCullFace(GL_BACK);
	}

	void SpotLight::setLight(const SpotLightComponent & light, const putils::Point3f & pos) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);
		putils::gl::setUniform(direction, light.direction);

		putils::gl::setUniform(cutOff, light.cutOff);
		putils::gl::setUniform(outerCutOff, light.outerCutOff);

		putils::gl::setUniform(diffuseStrength, light.diffuseStrength);
		putils::gl::setUniform(specularStrength, light.specularStrength);

		putils::gl::setUniform(attenuationConstant, light.constant);
		putils::gl::setUniform(attenuationLinear, light.linear);
		putils::gl::setUniform(attenuationQuadratic, light.quadratic);
	}
}
