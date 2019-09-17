#include "DirLight.hpp"

#include "EntityManager.hpp"
#include "helpers/LightHelper.hpp"
#include "shaders/shaders.hpp"

#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/ShaderComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

namespace kengine {
	extern float SHADOW_MAP_MIN_BIAS;
	extern float SHADOW_MAP_MAX_BIAS;
}

namespace kengine::Shaders {
	DirLight::DirLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(DirLight)),
		_em(em)
	{}

	void DirLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<DirLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::ShadowMap::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::DirLight::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(this->shadowMap, _shadowMapTextureID);

		putils::gl::setUniform(proj, glm::mat4(1.f));
		putils::gl::setUniform(view, glm::mat4(1.f));
		putils::gl::setUniform(model, glm::mat4(1.f));
	}

	void DirLight::run(const Parameters & params) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();

		putils::gl::setUniform(viewPos, params.camPos);
		putils::gl::setUniform(screenSize, params.screenSize);

		putils::gl::setUniform(shadow_map_min_bias, SHADOW_MAP_MIN_BIAS);
		putils::gl::setUniform(shadow_map_max_bias, SHADOW_MAP_MAX_BIAS);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto &[e, light] : _em.getEntities<DirLightComponent>()) {
			const putils::Point3f pPos = { params.camPos.x, params.camPos.y, params.camPos.z };

			for (const auto & [shadowMapEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowMapShaderComponent>()) {
				auto & shadowMap = static_cast<ShadowMapShader &>(*shader.shader);
				shadowMap.run(e, light, pPos, (size_t)params.screenSize.x, (size_t)params.screenSize.y);
			}

			use();

			setLight(light);

			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, params.camPos, (size_t)params.screenSize.x, (size_t)params.screenSize.y));

			ShaderHelper::shapes::drawQuad();
		}
	}

	void DirLight::setLight(const DirLightComponent & light) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(direction, light.direction);

		putils::gl::setUniform(ambientStrength, light.ambientStrength);
		putils::gl::setUniform(diffuseStrength, light.diffuseStrength);
		putils::gl::setUniform(specularStrength, light.specularStrength);
	}
}
