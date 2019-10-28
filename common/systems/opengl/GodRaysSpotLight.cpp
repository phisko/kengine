#include "GodRaysSpotLight.hpp"
#include "EntityManager.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GodRaysComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "ShadowMap.hpp"
#include "shaders/shaders.hpp"

namespace kengine::Shaders {
	GodRaysSpotLight::GodRaysSpotLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(GodRaysSpotLight)),
		_em(em)
	{
	}

	void GodRaysSpotLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysSpotLight>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::PointLight::GetDirection::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowMap::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRaysSpotLight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		putils::gl::setUniform(this->inverseView, glm::inverse(params.view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(params.proj));
		putils::gl::setUniform(this->viewPos, params.camPos);
		putils::gl::setUniform(this->screenSize, putils::Point2f(params.viewPort.size));

		for (const auto &[e, light, depthMap, transform, comp] : _em.getEntities<SpotLightComponent, DepthMapComponent, TransformComponent3f, GodRaysComponent>()) {
			putils::gl::setUniform(SCATTERING, comp.scattering);
			putils::gl::setUniform(NB_STEPS, comp.nbSteps);
			putils::gl::setUniform(DEFAULT_STEP_LENGTH, comp.defaultStepLength);
			putils::gl::setUniform(INTENSITY, comp.intensity);
			drawLight(params.camPos, light, transform.boundingBox.position, depthMap, (size_t)params.viewPort.size.x, (size_t)params.viewPort.size.y);
		}
	}

	void GodRaysSpotLight::drawLight(const glm::vec3 & camPos, const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);

		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, glm::vec3(pos.x, pos.y, pos.z), screenWidth, screenHeight));

		ShaderHelper::shapes::drawQuad();
	}
}
