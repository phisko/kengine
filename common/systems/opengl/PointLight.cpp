#include "PointLight.hpp"

#include "ShadowCube.hpp"
#include "EntityManager.hpp"
#include "shaders/shaders.hpp"

#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ShaderComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

namespace kengine::Shaders {
	void PointLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<PointLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::PointLight::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowCube::frag, GL_FRAGMENT_SHADER }
		));

		use();
		_shadowMapTextureID = firstTextureID;
		_shadowMap = _shadowMapTextureID;
	}

	void PointLight::run(const Parameters & params) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderHelper::Enable __c(GL_CULL_FACE);
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewPort.size);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto radius = LightHelper::getRadius(light);
			const auto & centre = transform.boundingBox.position;

			if (light.castShadows)
				for (const auto & [shadowCubeEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowCubeShaderComponent>()) {
					auto & shadowCube = static_cast<ShadowCubeShader &>(*shader.shader);
					shadowCube.run(e, light, centre, radius, params);
				}

			use();

			_proj = params.proj;
			_view = params.view;

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			model = glm::scale(model, { radius, radius, radius });
			_model = model;

			if (centre.getDistanceTo({ params.camPos.x, params.camPos.y, params.camPos.z }) < radius)
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
		_position = pos;

		_diffuseStrength = light.diffuseStrength;
		_specularStrength = light.specularStrength;

		_attenuationConstant = light.constant;
		_attenuationLinear = light.linear;
		_attenuationQuadratic = light.quadratic;
		_farPlane = radius;

		_bias = light.shadowMapBias;
	}
}
