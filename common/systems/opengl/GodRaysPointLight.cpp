#include "GodRaysPointLight.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "ShadowCube.hpp"
#include "shaders/shaders.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "helpers/ShaderHelper.hpp"

extern float SCATTERING_ADJUST;
extern float NB_STEPS_ADJUST;
extern float DEFAULT_STEP_LENGTH_ADJUST;
extern float INTENSITY_ADJUST;

namespace kengine::Shaders {
	GodRaysPointLight::GodRaysPointLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(GodRaysPointLight)),
		_em(em)
	{
	}

	void GodRaysPointLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysPointLight>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::PointLight::GetDirection::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowCube::frag, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRaysPointLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
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

		for (const auto &[e, light, depthMap, transform] : _em.getEntities<PointLightComponent, DepthCubeComponent, kengine::TransformComponent3f>())
			drawLight(camPos, light, transform.boundingBox.topLeft, depthMap, screenWidth, screenHeight);
	}

	void GodRaysPointLight::drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);

		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap.texture);
		putils::gl::setUniform(farPlane, LightHelper::getRadius(light));

		shapes::drawQuad();
	}
}
