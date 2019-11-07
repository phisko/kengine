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
			ShaderDescription{ src::CSM::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		for (size_t i = 0; i < lengthof(shadowMap); ++i)
			putils::gl::setUniform(shadowMap[i], _shadowMapTextureID + i);
	}

	void GodRaysDirLight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);


		putils::gl::setUniform(this->inverseView, glm::inverse(params.view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(params.proj));
		putils::gl::setUniform(this->viewPos, params.camPos);
		putils::gl::setUniform(this->screenSize, putils::Point2f(params.viewPort.size));

		for (const auto &[e, light, depthMap, comp] : _em.getEntities<DirLightComponent, CSMComponent, GodRaysComponent>()) {
			putils::gl::setUniform(SCATTERING, comp.scattering);
			putils::gl::setUniform(NB_STEPS, comp.nbSteps);
			putils::gl::setUniform(DEFAULT_STEP_LENGTH, comp.defaultStepLength);
			putils::gl::setUniform(INTENSITY, comp.intensity);

			drawLight(light, depthMap, params);
		}
	}

	void GodRaysDirLight::drawLight(const DirLightComponent & light, const CSMComponent & depthMap, const Parameters & params) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(direction, light.direction);

		for (int i = 0; i < lengthof(depthMap.textures); ++i) {
			glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID + i);
			glBindTexture(GL_TEXTURE_2D, depthMap.textures[i]);
			putils::gl::setUniform(lightSpaceMatrix[i], LightHelper::getCSMLightSpaceMatrix(light, params, i));
			putils::gl::setUniform(cascadeEnd[i], LightHelper::getCSMCascadeEnd(light, i));
		}

		ShaderHelper::shapes::drawQuad();
	}
}
