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
	float POINT_LIGHT_BIAS = .05f;

	void PointLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<PointLight>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::PointLight::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ShadowCube::frag, GL_FRAGMENT_SHADER }
		));

		use();
		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);

		_em += [](Entity & e) { e += AdjustableComponent("[Render/Lights] Shadow cube bias", &POINT_LIGHT_BIAS); };
	}

	void PointLight::run(const Parameters & params) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderHelper::Enable __c(GL_CULL_FACE);
		ShaderHelper::Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(bias, POINT_LIGHT_BIAS);
		putils::gl::setUniform(viewPos, params.camPos);
		putils::gl::setUniform(screenSize, params.screenSize);

		glActiveTexture((GLenum)(GL_TEXTURE0 + _shadowMapTextureID));

		for (auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto radius = LightHelper::getRadius(light);
			const auto & centre = transform.boundingBox.position;

			for (const auto & [shadowCubeEntity, shader, comp] : _em.getEntities<LightingShaderComponent, ShadowCubeShaderComponent>()) {
				auto & shadowCube = static_cast<ShadowCubeShader &>(*shader.shader);
				shadowCube.run(e, light, centre, radius, (size_t)params.screenSize.x, (size_t)params.screenSize.y);
			}

			use();

			putils::gl::setUniform(this->proj, params.proj);
			putils::gl::setUniform(this->view, params.view);

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			model = glm::scale(model, { radius, radius, radius });
			putils::gl::setUniform(this->model, model);

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
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);

		putils::gl::setUniform(diffuseStrength, light.diffuseStrength);
		putils::gl::setUniform(specularStrength, light.specularStrength);

		putils::gl::setUniform(attenuationConstant, light.constant);
		putils::gl::setUniform(attenuationLinear, light.linear);
		putils::gl::setUniform(attenuationQuadratic, light.quadratic);
		putils::gl::setUniform(farPlane, radius);
	}
}
