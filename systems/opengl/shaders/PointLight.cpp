#include "PointLight.hpp"

#include "ShadowCube.hpp"
#include "EntityManager.hpp"

#include "data/TransformComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "ShaderHelper.hpp"

namespace kengine::Shaders {
	void PointLight::init(size_t firstTextureID) {
		initWithShaders<PointLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::PointLight::Frag::glsl, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		use();
		_shadowMapTextureID = firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void PointLight::run(const Parameters & params) {
		ShaderHelper::Enable __c(GL_CULL_FACE);
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		src::ShadowCube::Frag::Uniforms::_viewPos = params.camPos;
		assert(src::ShadowCube::Frag::Uniforms::_viewPos.location == src::PointLight::Frag::Uniforms::_viewPos.location);
		_screenSize = putils::Point2f(params.viewport.size);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto & [e, light, transform] : _em.getEntities<PointLightComponent, TransformComponent>()) {
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto radius = LightHelper::getRadius(light);
			const auto & centre = transform.boundingBox.position;

			if (light.castShadows) {
				if (e.has<DepthCubeComponent>()) {
					ShaderHelper::BindFramebuffer b(e.get<DepthCubeComponent>().fbo);
					glClear(GL_DEPTH_BUFFER_BIT);
				}

				for (const auto & [shadowCubeEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowCubeShaderComponent>()) {
					auto & shadowCube = static_cast<ShadowCubeShader &>(*shader.shader);
					shadowCube.run(e, light, centre, radius, params);
				}
			}

			use();

			_proj = params.proj;
			_view = params.view;

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			model = glm::scale(model, { radius, radius, radius });
			_model = model;

			const auto centreToCam = putils::Point3f{ params.camPos.x, params.camPos.y, params.camPos.z } - centre;
			if (centre.getLengthSquared() < radius * radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			setLight(light, centre, radius);

			glBindTexture(GL_TEXTURE_CUBE_MAP, e.get<DepthCubeComponent>().texture);

			ShaderHelper::shapes::drawSphere();
		}

		glCullFace(GL_BACK);
	}

	void PointLight::setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius) {
		_color = light.color;
		src::ShadowCube::Frag::Uniforms::_position = pos;
		assert(src::ShadowCube::Frag::Uniforms::_position.location == src::PointLight::Frag::Uniforms::_position.location);

		_diffuseStrength = light.diffuseStrength;
		_specularStrength = light.specularStrength;

		_attenuationConstant = light.constant;
		_attenuationLinear = light.linear;
		_attenuationQuadratic = light.quadratic;
		_farPlane = radius;

		_bias = light.shadowMapBias;
	}
}
