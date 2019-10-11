#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
	struct DepthCubeComponent;
}

namespace kengine::Shaders {
	class GodRaysPointLight : public putils::gl::Program {
	public:
		GodRaysPointLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight);

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
		GLint farPlane;

		GLint shadowMap;
		GLint bias;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysPointLight::SCATTERING),
			pmeta_reflectible_attribute(&GodRaysPointLight::NB_STEPS),
			pmeta_reflectible_attribute(&GodRaysPointLight::DEFAULT_STEP_LENGTH),
			pmeta_reflectible_attribute(&GodRaysPointLight::INTENSITY),

			pmeta_reflectible_attribute(&GodRaysPointLight::inverseView),
			pmeta_reflectible_attribute(&GodRaysPointLight::inverseProj),
			pmeta_reflectible_attribute(&GodRaysPointLight::viewPos),
			pmeta_reflectible_attribute(&GodRaysPointLight::screenSize),

			pmeta_reflectible_attribute(&GodRaysPointLight::color),
			pmeta_reflectible_attribute(&GodRaysPointLight::position),
			pmeta_reflectible_attribute(&GodRaysPointLight::farPlane),

			pmeta_reflectible_attribute(&GodRaysPointLight::shadowMap),
			pmeta_reflectible_attribute(&GodRaysPointLight::bias)
		);
	};
}
