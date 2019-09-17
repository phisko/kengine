#include "GodRaysDirLight.hpp"
#include "EntityManager.hpp"

#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/ShaderComponent.hpp"
#include "components/GodRaysComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "shaders/shaders.hpp"
#include "ShadowMap.hpp"

namespace kengine::Shaders {
	GodRaysDirLight::GodRaysDirLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(GodRaysDirLight)),
		_em(em)
	{
	}

	void GodRaysDirLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysDirLight>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::DirLight::GetDirection::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowMap::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRaysDirLight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		putils::gl::setUniform(this->inverseView, glm::inverse(params.view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(params.proj));
		putils::gl::setUniform(this->viewPos, params.camPos);
		putils::gl::setUniform(this->screenSize, params.screenSize);

		for (const auto &[e, light, depthMap, comp] : _em.getEntities<DirLightComponent, DepthMapComponent, GodRaysComponent>()) {
			putils::gl::setUniform(SCATTERING, comp.scattering);
			putils::gl::setUniform(NB_STEPS, comp.nbSteps);
			putils::gl::setUniform(DEFAULT_STEP_LENGTH, comp.defaultStepLength);
			putils::gl::setUniform(INTENSITY, comp.intensity);
			drawLight(params.camPos, light, depthMap, (size_t)params.screenSize.x, (size_t)params.screenSize.y);
		}
	}

	void GodRaysDirLight::drawLight(const glm::vec3 & camPos, const DirLightComponent & light, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(direction, light.direction);

		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, camPos, screenWidth, screenHeight));

		ShaderHelper::shapes::drawQuad();
	}
}
