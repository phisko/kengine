#include "DirLight.hpp"

#include "Shapes.hpp"
#include "ShadowMap.hpp"
#include "EntityManager.hpp"
#include "RAII.hpp"
#include "LightHelper.hpp"
#include "shaders/shaders.hpp"

#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"

namespace kengine {
	extern float SHADOW_MAP_MIN_BIAS;
	extern float SHADOW_MAP_MAX_BIAS;
}

namespace kengine::Shaders {
	DirLight::DirLight(kengine::EntityManager & em, ShadowMap & shadowMap)
		: Program(true, pmeta_nameof(DirLight)),
		_em(em),
		_shadowMap(shadowMap)
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

	void DirLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();

		putils::gl::setUniform(viewPos, camPos);
		putils::gl::setUniform(screenSize, glm::vec2(screenWidth, screenHeight));

		putils::gl::setUniform(shadow_map_min_bias, SHADOW_MAP_MIN_BIAS);
		putils::gl::setUniform(shadow_map_max_bias, SHADOW_MAP_MAX_BIAS);

		for (auto &[e, light] : _em.getEntities<DirLightComponent>()) {
			const putils::Point3f pPos = { camPos.x, camPos.y, camPos.z };

			_shadowMap.run(e, light, pPos, screenWidth, screenHeight);
			use();

			setLight(light);

			glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);
			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, camPos, screenWidth, screenHeight));

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
