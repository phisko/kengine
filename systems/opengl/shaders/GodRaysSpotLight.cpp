#include "GodRaysSpotLight.hpp"
#include "kengine.hpp"

#include "data/LightComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/GodRaysComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/lightHelper.hpp"
#include "shaderHelper.hpp"

#include "ShadowMap.hpp"
#include "QuadSrc.hpp"

namespace kengine::opengl::shaders {
	GodRaysSpotLight::GodRaysSpotLight() noexcept
		: Program(true, putils_nameof(GodRaysSpotLight))
	{
	}

	void GodRaysSpotLight::init(size_t firstTextureID) noexcept {
		initWithShaders<GodRaysSpotLight>(putils::make_vector(
			ShaderDescription{ src::Quad::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::PointLight::GetDirection::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowMap::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void GodRaysSpotLight::run(const Parameters & params) noexcept {
		use();

		shaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		_inverseView = glm::inverse(params.view);
		_inverseProj = glm::inverse(params.proj);
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewport.size);

		for (const auto &[e, light, depthMap, transform, comp] : entities.with<SpotLightComponent, DepthMapComponent, TransformComponent, GodRaysComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_scattering = comp.scattering;
			_nbSteps = comp.nbSteps;
			_defaultStepLength = comp.defaultStepLength;
			_intensity = comp.intensity;
			drawLight(light, transform.boundingBox.position, depthMap, params);
		}
	}

	void GodRaysSpotLight::drawLight(const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, const Parameters & params) noexcept {
		_color = light.color;
		_position = pos;

		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		_lightSpaceMatrix = lightHelper::getLightSpaceMatrix(light, glm::vec3(pos.x, pos.y, pos.z), params);

		shaderHelper::shapes::drawQuad();
	}
}
