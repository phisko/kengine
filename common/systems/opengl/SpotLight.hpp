#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap;

	class SpotLight : public putils::gl::Program {
	public:
		SpotLight(kengine::EntityManager & em, ShadowMap & shadowMap)
			: Program(true, pmeta_nameof(SpotLight)),
			_em(em),
			_shadowMap(shadowMap)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint lightSpaceMatrix;
		GLint shadowMap;

		GLint viewPos;

		GLint color;
		GLint position;
		GLint direction;

		GLint cutOff;
		GLint outerCutOff;

		GLint diffuseStrength;
		GLint specularStrength;

		GLint attenuationConstant;
		GLint attenuationLinear;
		GLint attenuationQuadratic;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpotLight::proj),
			pmeta_reflectible_attribute(&SpotLight::view),
			pmeta_reflectible_attribute(&SpotLight::model),

			pmeta_reflectible_attribute(&SpotLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&SpotLight::shadowMap),

			pmeta_reflectible_attribute(&SpotLight::viewPos),

			pmeta_reflectible_attribute(&SpotLight::color),
			pmeta_reflectible_attribute(&SpotLight::position),
			pmeta_reflectible_attribute(&SpotLight::direction),

			pmeta_reflectible_attribute(&SpotLight::cutOff),
			pmeta_reflectible_attribute(&SpotLight::outerCutOff),

			pmeta_reflectible_attribute(&SpotLight::diffuseStrength),
			pmeta_reflectible_attribute(&SpotLight::specularStrength),

			pmeta_reflectible_attribute(&SpotLight::attenuationConstant),
			pmeta_reflectible_attribute(&SpotLight::attenuationLinear),
			pmeta_reflectible_attribute(&SpotLight::attenuationQuadratic)
		);

	private:
		kengine::EntityManager & _em;
		ShadowMap & _shadowMap;
		size_t _shadowMapTextureID;

		void setLight(const SpotLightComponent & light, const putils::Point3f & pos); 
	};
}