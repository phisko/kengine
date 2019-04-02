#include "LightSphere.hpp"
#include "EntityManager.hpp"

#include "Shapes.hpp"
#include "RAII.hpp"
#include "shaders/shaders.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/AdjustableComponent.hpp"

static auto SPHERE_SIZE = .25f;
static auto SUN_DIST = 500.f;
static auto SUN_SIZE = 100.f;

namespace kengine::Shaders {
	LightSphere::LightSphere(kengine::EntityManager & em) 
		: Program(false, pmeta_nameof(LightSphere)),
		_em(em) {
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Sphere size", &SPHERE_SIZE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Sun size", &SUN_SIZE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Sun dist", &SUN_DIST); };
	}

	void LightSphere::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<LightSphere>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::Color::frag, GL_FRAGMENT_SHADER }
		));

		_gBufferFBO = gBufferFBO;
	}

	void LightSphere::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _gBufferFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Enable __e(GL_DEPTH_TEST);
		Enable __c(GL_CULL_FACE);

		use();
		putils::gl::setUniform(this->proj, proj);
		putils::gl::setUniform(this->view, view);

		for (const auto & [e, light] : _em.getEntities<DirLightComponent>())
			drawLight(light, camPos - light.direction * SUN_DIST, SUN_SIZE);

		for (const auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.topLeft;
			drawLight(light, { pos.x, pos.y, pos.z }, SPHERE_SIZE);
		}

		for (const auto & [e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.topLeft;
			const bool isFacingLight = glm::dot({ pos.x, pos.y, pos.z }, camPos) < 0;
			if (isFacingLight)
				drawLight(light, { pos.x, pos.y, pos.z }, SPHERE_SIZE);
		}
	}

	void LightSphere::drawLight(const LightComponent & light, const glm::vec3 & pos, float size) {
		glm::mat4 model(1.f);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(size));
		putils::gl::setUniform(this->model, model);
		putils::gl::setUniform(this->color, light.color);

		shapes::drawSphere();
	}

}
