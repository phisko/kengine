#include "GodRaysDirLight.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "ShadowMap.hpp"
#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "RAII.hpp"
#include "LightHelper.hpp"
#include "shaders/shaders.hpp"

auto SCATTERING_ADJUST = 0.f;
auto NB_STEPS_ADJUST = 50.f;
auto DEFAULT_STEP_LENGTH_ADJUST = 5.f;
auto INTENSITY_ADJUST = 1.f;

namespace kengine::Shaders {
	GodRaysDirLight::GodRaysDirLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(GodRaysDirLight)),
		_em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Scattering", &SCATTERING_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Nb steps", &NB_STEPS_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Default step length", &DEFAULT_STEP_LENGTH_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Intensity", &INTENSITY_ADJUST); };
	}

	void GodRaysDirLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysDirLight>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::DirLight::GetDirection::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowMap::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRaysDirLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		putils::gl::setUniform(SCATTERING, SCATTERING_ADJUST);
		putils::gl::setUniform(NB_STEPS, NB_STEPS_ADJUST);
		putils::gl::setUniform(DEFAULT_STEP_LENGTH, DEFAULT_STEP_LENGTH_ADJUST);
		putils::gl::setUniform(INTENSITY, INTENSITY_ADJUST);

		putils::gl::setUniform(this->inverseView, glm::inverse(view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(proj));
		putils::gl::setUniform(this->viewPos, camPos);
		putils::gl::setUniform(this->screenSize, glm::vec2(screenWidth, screenHeight));

		for (const auto &[e, light, depthMap] : _em.getEntities<DirLightComponent, DepthMapComponent>())
			drawLight(camPos, light, depthMap, screenWidth, screenHeight);
	}

	void GodRaysDirLight::drawLight(const glm::vec3 & camPos, const DirLightComponent & light, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(direction, light.direction);

		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		putils::gl::setUniform(lightSpaceMatrix, LightHelper::getLightSpaceMatrix(light, camPos, screenWidth, screenHeight));

		shapes::drawQuad();
	}
}
