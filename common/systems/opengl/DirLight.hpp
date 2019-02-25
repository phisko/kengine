#pragma once

#include "opengl/Program.hpp"
#include "SSAO.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap;

	class DirLight : public putils::gl::Program {
	public:
		DirLight(kengine::EntityManager & em, ShadowMap & shadowMap, SSAO & ssao, SSAOBlur & ssaoBlur);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint lightSpaceMatrix;
		GLint shadowMap;
		GLint ssao;

		GLint runSSAO;

		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint direction;

		GLint ambientStrength;
		GLint diffuseStrength;
		GLint specularStrength;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLight::proj),
			pmeta_reflectible_attribute(&DirLight::view),
			pmeta_reflectible_attribute(&DirLight::model),

			pmeta_reflectible_attribute(&DirLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&DirLight::shadowMap),
			pmeta_reflectible_attribute(&DirLight::ssao),

			pmeta_reflectible_attribute(&DirLight::runSSAO),

			pmeta_reflectible_attribute(&DirLight::viewPos),
			pmeta_reflectible_attribute(&DirLight::screenSize),

			pmeta_reflectible_attribute(&DirLight::color),
			pmeta_reflectible_attribute(&DirLight::direction),

			pmeta_reflectible_attribute(&DirLight::ambientStrength),
			pmeta_reflectible_attribute(&DirLight::diffuseStrength),
			pmeta_reflectible_attribute(&DirLight::specularStrength)
		);

	private:
		kengine::EntityManager & _em;
		ShadowMap & _shadowMap;
		size_t _shadowMapTextureID;
		size_t _ssaoTextureID;

	private:
		SSAO & _ssao;
		SSAOBlur & _ssaoBlur;

		void setLight(const DirLightComponent & light); 
	};
}
