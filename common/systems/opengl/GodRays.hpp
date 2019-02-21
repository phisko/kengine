#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	struct DepthMapComponent;

	class GodRays : public putils::gl::Program {
	public:
		GodRays(kengine::EntityManager & em);

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

		GLint inverseView;
		GLint inverseProj;
		GLint viewPos;

		GLint color;
		GLint direction;

		GLint shadowMap;
		GLint lightSpaceMatrix;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRays::SCATTERING),
			pmeta_reflectible_attribute(&GodRays::NB_STEPS),
			pmeta_reflectible_attribute(&GodRays::DEFAULT_STEP_LENGTH),

			pmeta_reflectible_attribute(&GodRays::inverseView),
			pmeta_reflectible_attribute(&GodRays::inverseProj),
			pmeta_reflectible_attribute(&GodRays::viewPos),

			pmeta_reflectible_attribute(&GodRays::color),
			pmeta_reflectible_attribute(&GodRays::direction),

			pmeta_reflectible_attribute(&GodRays::shadowMap),
			pmeta_reflectible_attribute(&GodRays::lightSpaceMatrix)
		);
	};
}
