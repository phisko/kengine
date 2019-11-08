#include "GodRaysSpotLight.hpp"
#include "EntityManager.hpp"

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GodRaysComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "ShadowMap.hpp"
#include "shaders/QuadSrc.hpp"

namespace kengine::Shaders {
	GodRaysSpotLight::GodRaysSpotLight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(GodRaysSpotLight)),
		_em(em)
	{
	}

	void GodRaysSpotLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysSpotLight>(putils::make_vector(
			ShaderDescription{ src::Quad::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::PointLight::GetDirection::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowMap::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void GodRaysSpotLight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		_inverseView = glm::inverse(params.view);
		_inverseProj = glm::inverse(params.proj);
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewPort.size);

		for (const auto &[e, light, depthMap, transform, comp] : _em.getEntities<SpotLightComponent, DepthMapComponent, TransformComponent3f, GodRaysComponent>()) {
			_scattering = comp.scattering;
			_nbSteps = comp.nbSteps;
			_defaultStepLength = comp.defaultStepLength;
			_intensity = comp.intensity;
			drawLight(light, transform.boundingBox.position, depthMap, params);
		}
	}

	void GodRaysSpotLight::drawLight(const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, const Parameters & params) {
		_color = light.color;
		_position = pos;

		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		_lightSpaceMatrix = LightHelper::getLightSpaceMatrix(light, glm::vec3(pos.x, pos.y, pos.z), params);

		ShaderHelper::shapes::drawQuad();
	}
}
