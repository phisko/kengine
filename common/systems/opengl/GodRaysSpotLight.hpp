#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
	struct DepthMapComponent;
}

namespace kengine::Shaders {
	class GodRaysSpotLight : public putils::gl::Program {
	public:
		GodRaysSpotLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, size_t screenWidth, size_t screenHeight);

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
		GLint position;

		GLint shadowMap;
		GLint lightSpaceMatrix;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysSpotLight::SCATTERING),
			pmeta_reflectible_attribute(&GodRaysSpotLight::NB_STEPS),
			pmeta_reflectible_attribute(&GodRaysSpotLight::DEFAULT_STEP_LENGTH),
			pmeta_reflectible_attribute(&GodRaysSpotLight::INTENSITY),

			pmeta_reflectible_attribute(&GodRaysSpotLight::inverseView),
			pmeta_reflectible_attribute(&GodRaysSpotLight::inverseProj),
			pmeta_reflectible_attribute(&GodRaysSpotLight::viewPos),
			pmeta_reflectible_attribute(&GodRaysSpotLight::screenSize),

			pmeta_reflectible_attribute(&GodRaysSpotLight::color),
			pmeta_reflectible_attribute(&GodRaysSpotLight::position),

			pmeta_reflectible_attribute(&GodRaysSpotLight::shadowMap),
			pmeta_reflectible_attribute(&GodRaysSpotLight::lightSpaceMatrix)
		);
	};
}
