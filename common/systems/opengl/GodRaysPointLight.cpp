#include "GodRaysPointLight.hpp"
#include "EntityManager.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GodRaysComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "ShadowCube.hpp"
#include "shaders/shaders.hpp"

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

		_shadowMapTextureID = (GLuint)firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void GodRaysPointLight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		putils::gl::setUniform(this->inverseView, glm::inverse(params.view));
		putils::gl::setUniform(this->inverseProj, glm::inverse(params.proj));
		putils::gl::setUniform(this->viewPos, params.camPos);
		putils::gl::setUniform(this->screenSize, putils::Point2f(params.viewPort.size));

		for (const auto &[e, light, depthMap, transform, comp] : _em.getEntities<PointLightComponent, DepthCubeComponent, TransformComponent3f, GodRaysComponent>()) {
			putils::gl::setUniform(SCATTERING, comp.scattering);
			putils::gl::setUniform(NB_STEPS, comp.nbSteps);
			putils::gl::setUniform(DEFAULT_STEP_LENGTH, comp.defaultStepLength);
			putils::gl::setUniform(INTENSITY, comp.intensity);
			drawLight(params.camPos, light, transform.boundingBox.position, depthMap, (size_t)params.viewPort.size.x, (size_t)params.viewPort.size.y);
		}
	}

	void GodRaysPointLight::drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);

		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap.texture);
		putils::gl::setUniform(farPlane, LightHelper::getRadius(light));

		ShaderHelper::shapes::drawQuad();
	}
}
