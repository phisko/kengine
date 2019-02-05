#include "GodRays.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "GodRaysFirstPass.hpp"
#include "components/LightComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/TransformComponent.hpp"
#include "RAII.hpp"

static auto EXPOSURE = .002f;
static auto DECAY = .99f;
static auto DENSITY = 1.f;
static auto WEIGHT = 5.f;
static auto NUM_SAMPLES = 100.f;

static auto SPHERE_SIZE = .25f;
static auto SUN_DIST = 500.f;
static auto SUN_SIZE = 100.f;

namespace kengine::Shaders {
	GodRays::GodRays(kengine::EntityManager & em, GodRaysFirstPass & firstPass)
		: _em(em), _firstPass(firstPass)
	{
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Sphere size", &SPHERE_SIZE); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Sun dist", &SUN_DIST); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Sun size", &SUN_SIZE); };

		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Exposure", &EXPOSURE); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Decay", &DECAY); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Density", &DENSITY); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Weight", &WEIGHT); };
		em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/God Rays] Num samples", &NUM_SAMPLES); };
	}


	void GodRays::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRays>(putils::make_vector(
			ShaderDescription{ "shaders/quad.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/godRays.frag", GL_FRAGMENT_SHADER }
		));

		_firstPassTextureID = firstTextureID;
		putils::gl::setUniform(firstPass, _firstPassTextureID);
	}

	void GodRays::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();
		putils::gl::setUniform(exposure, EXPOSURE);
		putils::gl::setUniform(decay, DECAY);
		putils::gl::setUniform(density, DENSITY);
		putils::gl::setUniform(weight, WEIGHT);
		putils::gl::setUniform(numSamples, NUM_SAMPLES);

		glActiveTexture(GL_TEXTURE0 + _firstPassTextureID);
		glBindTexture(GL_TEXTURE_2D, _firstPass.getTexture());

		for (const auto &[e, light] : _em.getEntities<DirLightComponent>())
			drawLight(camPos - light.direction * SUN_DIST, SUN_SIZE, light.color, view, proj, screenWidth, screenHeight);

		for (const auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.topLeft;
			drawLight({ pos.x, pos.y, pos.z }, SPHERE_SIZE, light.color, view, proj, screenWidth, screenHeight);
		}

		for (const auto & [e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.topLeft;

			const bool isFacingLight = glm::dot({ pos.x, pos.y, pos.z }, camPos) < 0;
			if (isFacingLight)
				drawLight({ pos.x, pos.y, pos.z }, SPHERE_SIZE, light.color, view, proj, screenWidth, screenHeight);
		}
	}

	static glm::vec2 getPosOnScreen(const glm::vec3 & pos, const glm::mat4 & proj, const glm::mat4 & view, size_t screenWidth, size_t screenHeight) {
		const auto newPos = proj * view * glm::vec4(pos, 1.f);
		const auto afterPerspective = glm::vec3(newPos.x, newPos.y, newPos.z) / newPos.w;
		return { (afterPerspective.x + 1.f) / 2.f, (afterPerspective.y + 1.f) / 2.f };
	}

	void GodRays::drawLight(const glm::vec3 & lightPos, float size, const glm::vec3 & color, const glm::mat4 & view, const glm::mat4 & proj, size_t screenWidth, size_t screenHeight) {
		glm::mat4 model(1.f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(size));

		_firstPass.run(view, proj, model, color);

		Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		const auto lightPosOnScreen = getPosOnScreen(lightPos, proj, view, screenWidth, screenHeight);
		putils::gl::setUniform(lightPositionOnScreen, glm::vec2{ lightPosOnScreen.x, lightPosOnScreen.y });
		shapes::drawQuad();
	}
}
