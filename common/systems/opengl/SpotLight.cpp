#include "SpotLight.hpp"

#include "ShadowMap.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "RAII.hpp"

#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"

namespace kengine::Shaders {
	void SpotLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SpotLight>(putils::make_vector(
			ShaderDescription{ "shaders/3d.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/shadowMap.frag", GL_FRAGMENT_SHADER },
			ShaderDescription{ "shaders/spotLight.frag", GL_FRAGMENT_SHADER }
		));

		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(this->shadowMap, _shadowMapTextureID);
	}

	void SpotLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Enable __c(GL_CULL_FACE);
		Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(viewPos, camPos);
		for (auto &[e, light, transform] : _em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
			const auto & centre = transform.boundingBox.topLeft;
			_shadowMap.run(e, light, centre, screenWidth, screenHeight);
			use();

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			const auto radius = getRadius(light);
			model = glm::scale(model, { radius, radius, radius });
			putils::gl::setUniform(this->proj, proj);
			putils::gl::setUniform(this->view, view);
			putils::gl::setUniform(this->model, model);

			if (centre.distanceTo(putils::Point3f{ camPos.x, camPos.y, camPos.z }) < radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			setLight(light, centre);

			glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);
			glBindTexture(GL_TEXTURE_2D, e.get<DepthMapComponent>().texture);
			putils::gl::setUniform(lightSpaceMatrix, getLightSpaceMatrix(light, { centre.x, centre.y, centre.z }, screenWidth, screenHeight));

			shapes::drawSphere();
		}

		glCullFace(GL_BACK);
	}

	void SpotLight::setLight(const SpotLightComponent & light, const putils::Point3f & pos) {
		putils::gl::setUniform(color, light.color);
		putils::gl::setUniform(position, pos);
		putils::gl::setUniform(direction, light.direction);

		putils::gl::setUniform(cutOff, light.cutOff);
		putils::gl::setUniform(outerCutOff, light.outerCutOff);

		putils::gl::setUniform(diffuseStrength, light.diffuseStrength);
		putils::gl::setUniform(specularStrength, light.specularStrength);

		putils::gl::setUniform(attenuationConstant, light.constant);
		putils::gl::setUniform(attenuationLinear, light.linear);
		putils::gl::setUniform(attenuationQuadratic, light.quadratic);
	}
}
