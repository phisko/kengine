#include "GodRays.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "ShadowMap.hpp"
#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/TransformComponent.hpp"
#include "RAII.hpp"

static auto SCATTERING_ADJUST = .1f;
static auto NB_STEPS_ADJUST = 100.f;
static auto DEFAULT_STEP_LENGTH_ADJUST = 10.f;

namespace kengine::Shaders {
	GodRays::GodRays(kengine::EntityManager & em) : Program(true), _em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Scattering", &SCATTERING_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Nb steps", &NB_STEPS_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/God Rays] Default step length", &DEFAULT_STEP_LENGTH_ADJUST); };
	}

	void GodRays::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRays>(putils::make_vector(
			ShaderDescription{ "shaders/quad.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/godRays.frag", GL_FRAGMENT_SHADER },
			ShaderDescription{ "shaders/shadow.frag", GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRays::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		putils::gl::setUniform(this->inverseView, glm::inverse(view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(proj));
		putils::gl::setUniform(this->viewPos, camPos);

		for (const auto &[e, light, depthMap] : _em.getEntities<DirLightComponent, DepthMapComponent>())
			drawLight(camPos, light, depthMap, screenWidth, screenHeight);

		// for (const auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
		// 	const auto & pos = transform.boundingBox.topLeft;
		// 	drawLight({ pos.x, pos.y, pos.z }, SPHERE_SIZE, light.color, view, proj, screenWidth, screenHeight);
		// }

		// for (const auto & [e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
		// 	const auto & pos = transform.boundingBox.topLeft;

		// 	const bool isFacingLight = glm::dot({ pos.x, pos.y, pos.z }, camPos) < 0;
		// 	if (isFacingLight)
		// 		drawLight({ pos.x, pos.y, pos.z }, SPHERE_SIZE, light.color, view, proj, screenWidth, screenHeight);
		// }
	}

	void GodRays::drawLight(const glm::vec3 & camPos, const DirLightComponent & light, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(SCATTERING, SCATTERING_ADJUST);
		putils::gl::setUniform(NB_STEPS, NB_STEPS_ADJUST);
		putils::gl::setUniform(DEFAULT_STEP_LENGTH, DEFAULT_STEP_LENGTH_ADJUST);

		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(direction, light.direction);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);
		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		putils::gl::setUniform(lightSpaceMatrix, getLightSpaceMatrix(light, camPos, screenWidth, screenHeight));

		shapes::drawQuad();
	}
}
