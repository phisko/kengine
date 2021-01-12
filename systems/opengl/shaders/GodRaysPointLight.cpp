#include "GodRaysPointLight.hpp"
#include "kengine.hpp"

#include "data/LightComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/GodRaysComponent.hpp"
#include "data/OpenGLResourceComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/lightHelper.hpp"
#include "shaderHelper.hpp"

#include "ShadowCube.hpp"
#include "QuadSrc.hpp"

namespace kengine::opengl::shaders {
	GodRaysPointLight::GodRaysPointLight() noexcept
		: Program(true, putils_nameof(GodRaysPointLight))
	{
	}

	void GodRaysPointLight::init(size_t firstTextureID) noexcept {
		initWithShaders<GodRaysPointLight>(putils::make_vector(
			ShaderDescription{ src::Quad::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::GodRays::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::PointLight::GetDirection::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = (GLuint)firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void GodRaysPointLight::run(const Parameters & params) noexcept {
		use();

		shaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);

		_inverseView = glm::inverse(params.view);
		_inverseProj = glm::inverse(params.proj);
		src::ShadowCube::Frag::Uniforms::_viewPos = params.camPos;
		assert(src::ShadowCube::Frag::Uniforms::_viewPos.location == src::GodRays::Frag::Uniforms::_viewPos.location);
		_screenSize = putils::Point2f(params.viewport.size);

		for (const auto &[e, light, depthMap, transform, comp] : entities.with<PointLightComponent, DepthCubeComponent, TransformComponent, GodRaysComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_scattering = comp.scattering;
			_nbSteps = comp.nbSteps;
			_defaultStepLength = comp.defaultStepLength;
			_intensity = comp.intensity;
			drawLight(params.camPos, light, transform.boundingBox.position, depthMap, (size_t)params.viewport.size.x, (size_t)params.viewport.size.y);
		}
	}

	void GodRaysPointLight::drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight) noexcept {
		_color = light.color;
		src::ShadowCube::Frag::Uniforms::_position = pos;
		assert(src::ShadowCube::Frag::Uniforms::_position.location == src::PointLight::GetDirection::Uniforms::_position.location);

		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap.texture);
		_farPlane = lightHelper::getRadius(light);

		shaderHelper::shapes::drawQuad();
	}
}
