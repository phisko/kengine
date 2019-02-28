#include "DirLight.hpp"

#include "Shapes.hpp"
#include "ShadowMap.hpp"
#include "EntityManager.hpp"
#include "RAII.hpp"
#include "LightHelper.hpp"

#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"

static bool RUN_SSAO = false;
namespace kengine::Shaders {
	DirLight::DirLight(kengine::EntityManager & em, ShadowMap & shadowMap, SSAO & ssao, SSAOBlur & ssaoBlur)
		: Program(true, pmeta_nameof(DirLight)),
		_em(em),
		_shadowMap(shadowMap),
		_ssao(ssao),
		_ssaoBlur(ssaoBlur)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/SSAO] Active", &RUN_SSAO); };
	}

	void DirLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<DirLight>(putils::make_vector(
			ShaderDescription{ "shaders/3d.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/shadowMap.frag", GL_FRAGMENT_SHADER },
			ShaderDescription{ "shaders/dirLight.frag", GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(this->shadowMap, _shadowMapTextureID);

		_ssaoTextureID = _shadowMapTextureID + 1;
		putils::gl::setUniform(ssao, _ssaoTextureID);

		putils::gl::setUniform(proj, glm::mat4(1.f));
		putils::gl::setUniform(view, glm::mat4(1.f));
		putils::gl::setUniform(model, glm::mat4(1.f));
	}

	void DirLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		if (RUN_SSAO) {
			_ssao.run(view, proj, screenWidth, screenHeight);
			_ssaoBlur.run(_ssao.getTexture(), screenWidth, screenHeight);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Enable __c(GL_CULL_FACE);
		Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(viewPos, camPos);
		putils::gl::setUniform(screenSize, glm::vec2(screenWidth, screenHeight));
		for (auto &[e, light] : _em.getEntities<DirLightComponent>()) {
			const putils::Point3f pPos = { camPos.x, camPos.y, camPos.z };

			_shadowMap.run(e, light, pPos, screenWidth, screenHeight);
			use();

			setLight(light);

			glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);
			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, camPos, screenWidth, screenHeight));

			putils::gl::setUniform(runSSAO, RUN_SSAO ? 1u : 0u);
			if (RUN_SSAO) {
				glActiveTexture(GL_TEXTURE0 + _ssaoTextureID);
				glBindTexture(GL_TEXTURE_2D, _ssaoBlur.getTexture());
			}

			shapes::drawQuad();
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
