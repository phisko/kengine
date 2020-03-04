#include "LightSphere.hpp"
#include "EntityManager.hpp"

#include "data/LightComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/AdjustableComponent.hpp"

#include "ShaderHelper.hpp"

#include "ColorSrc.hpp"

static auto SPHERE_SIZE = .25f;
static auto SUN_DIST = 500.f;
static auto SUN_SIZE = 100.f;

namespace kengine::Shaders {
	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	LightSphere::LightSphere(EntityManager & em, Entity & parent) 
		: Program(false, putils_nameof(LightSphere)),
		_em(em) {
		parent += AdjustableComponent{
			"Render/Lights", {
				{ "Sphere size", &SPHERE_SIZE },
				{ "Sun size", &SUN_SIZE },
				{ "Sun dist", &SUN_DIST }
			}
		};
	}

	void LightSphere::init(size_t firstTextureID) {
		initWithShaders<LightSphere>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::Color::Frag::glsl, GL_FRAGMENT_SHADER }
		));
	}

	void LightSphere::run(const Parameters & params) {
		ShaderHelper::Enable __e(GL_DEPTH_TEST);
		ShaderHelper::Enable __c(GL_CULL_FACE);

		use();
		_proj = params.proj;
		_view = params.view;

		for (const auto & [e, light] : _em.getEntities<DirLightComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			drawLight(light, params.camPos - toVec(light.direction) * SUN_DIST, SUN_SIZE);
		}

		for (const auto & [e, light, transform] : _em.getEntities<PointLightComponent, TransformComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & pos = transform.boundingBox.position;
			drawLight(light, toVec(pos), SPHERE_SIZE);
		}

		for (const auto & [e, light, transform] : _em.getEntities<SpotLightComponent, TransformComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

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
		_model = model;
		_color = light.color;

		ShaderHelper::shapes::drawSphere();
	}
}
