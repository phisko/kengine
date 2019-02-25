#include "PointLight.hpp"

#include "ShadowCube.hpp"
#include "Shapes.hpp"
#include "EntityManager.hpp"
#include "RAII.hpp"

#include "components/TransformComponent.hpp"
#include "components/LightComponent.hpp"

namespace kengine::Shaders {
	void PointLight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<PointLight>(putils::make_vector(
			ShaderDescription{ "shaders/3d.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/pointLight.frag", GL_FRAGMENT_SHADER },
			ShaderDescription{ "shaders/shadowCube.frag", GL_FRAGMENT_SHADER }
		));

		use();
		_shadowMapTextureID = firstTextureID;
		putils::gl::setUniform(shadowMap, _shadowMapTextureID);
	}

	void PointLight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Enable __c(GL_CULL_FACE);
		Enable __b(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		use();
		putils::gl::setUniform(viewPos, camPos);
		putils::gl::setUniform(screenSize, glm::vec2(screenWidth, screenHeight));

		for (auto & [e, light, transform] : _em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
			const auto radius = getRadius(light);
			const auto & centre = transform.boundingBox.topLeft;

			_shadowCube.run(e, light, centre, radius, screenWidth, screenHeight);
			use();

			putils::gl::setUniform(this->proj, proj);
			putils::gl::setUniform(this->view, view);

			glm::mat4 model(1.f);
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			model = glm::scale(model, { radius, radius, radius });
			putils::gl::setUniform(this->model, model);

			if (centre.distanceTo(putils::Point3f{ camPos.x, camPos.y, camPos.z }) < radius)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);

			setLight(light, centre, radius);

			glActiveTexture(GL_TEXTURE0 + _shadowMapTextureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, e.get<DepthCubeComponent>().texture);

			shapes::drawSphere();
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
