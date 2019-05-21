#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
	struct DepthMapComponent;
}

namespace kengine::Shaders {
	class GodRaysDirLight : public putils::gl::Program {
	public:
		GodRaysDirLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const DirLightComponent & light, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight);

	private:
		kengine::EntityManager & _em;
		GLuint _shadowMapTextureID;

	public:
		GLint SCATTERING;
		GLint NB_STEPS;
		GLint DEFAULT_STEP_LENGTH;
		GLint INTENSITY;

		GLint inverseView;
		GLint inverseProj;
		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint direction;

		GLint shadowMap;
		GLint lightSpaceMatrix;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysDirLight::SCATTERING),
			pmeta_reflectible_attribute(&GodRaysDirLight::NB_STEPS),
			pmeta_reflectible_attribute(&GodRaysDirLight::DEFAULT_STEP_LENGTH),
			pmeta_reflectible_attribute(&GodRaysDirLight::INTENSITY),

			pmeta_reflectible_attribute(&GodRaysDirLight::inverseView),
			pmeta_reflectible_attribute(&GodRaysDirLight::inverseProj),
			pmeta_reflectible_attribute(&GodRaysDirLight::viewPos),
			pmeta_reflectible_attribute(&GodRaysDirLight::screenSize),

			pmeta_reflectible_attribute(&GodRaysDirLight::color),
			pmeta_reflectible_attribute(&GodRaysDirLight::direction),

			pmeta_reflectible_attribute(&GodRaysDirLight::shadowMap),
			pmeta_reflectible_attribute(&GodRaysDirLight::lightSpaceMatrix)
		);
	};
}
