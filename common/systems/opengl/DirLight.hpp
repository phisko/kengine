#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap;

	class DirLight : public putils::gl::Program {
	public:
		DirLight(kengine::EntityManager & em, ShadowMap & shadowMap);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	private:
		void setLight(const DirLightComponent & light); 

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint lightSpaceMatrix;
		GLint shadowMap;

		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint direction;

		GLint ambientStrength;
		GLint diffuseStrength;
		GLint specularStrength;

		GLint shadow_map_min_bias;
		GLint shadow_map_max_bias;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLight::proj),
			pmeta_reflectible_attribute(&DirLight::view),
			pmeta_reflectible_attribute(&DirLight::model),

			pmeta_reflectible_attribute(&DirLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&DirLight::shadowMap),

			pmeta_reflectible_attribute(&DirLight::viewPos),
			pmeta_reflectible_attribute(&DirLight::screenSize),

			pmeta_reflectible_attribute(&DirLight::color),
			pmeta_reflectible_attribute(&DirLight::direction),

			pmeta_reflectible_attribute(&DirLight::ambientStrength),
			pmeta_reflectible_attribute(&DirLight::diffuseStrength),
			pmeta_reflectible_attribute(&DirLight::specularStrength),

			pmeta_reflectible_attribute(&DirLight::shadow_map_min_bias),
			pmeta_reflectible_attribute(&DirLight::shadow_map_max_bias)
		);

	private:
		kengine::EntityManager & _em;
		ShadowMap & _shadowMap;
		size_t _shadowMapTextureID;
	};
}
