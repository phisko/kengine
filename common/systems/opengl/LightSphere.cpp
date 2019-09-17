#include "LightSphere.hpp"
#include "EntityManager.hpp"

#include "shaders/shaders.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

static auto SPHERE_SIZE = .25f;
static auto SUN_DIST = 500.f;
static auto SUN_SIZE = 100.f;

namespace kengine::Shaders {
	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

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

	void LightSphere::run(const Parameters & params) {
		ShaderHelper::Enable __e(GL_DEPTH_TEST);
		ShaderHelper::Enable __c(GL_CULL_FACE);

		use();
		putils::gl::setUniform(this->proj, params.proj);
		putils::gl::setUniform(this->view, params.view);

		for (const auto & [e, light] : _em.getEntities<DirLightComponent>())
			drawLight(light, params.camPos - toVec(light.direction) * SUN_DIST, SUN_SIZE);

		for (const auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.position;
			drawLight(light, toVec(pos), SPHERE_SIZE);
		}

		for (const auto & [e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & pos = transform.boundingBox.position;
			const bool isFacingLight = glm::dot(toVec(pos), params.camPos) < 0;
			if (isFacingLight)
				drawLight(light, toVec(pos), SPHERE_SIZE);
		}
	}

	void LightSphere::drawLight(const LightComponent & light, const glm::vec3 & pos, float size) {
		glm::mat4 model(1.f);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(size));
		putils::gl::setUniform(this->model, model);
		putils::gl::setUniform(this->color, light.color);

		ShaderHelper::shapes::drawSphere();
	}
}
