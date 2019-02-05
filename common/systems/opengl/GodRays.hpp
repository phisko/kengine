#pragma once

#include "opengl/Program.hpp"

namespace kengine { class EntityManager; }

namespace kengine::Shaders {
	class GodRaysFirstPass;

	class GodRays : public putils::gl::Program {
	public:
		GodRays(kengine::EntityManager & em, GodRaysFirstPass & firstPass);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;
		
	private:
		void drawLight(const glm::vec3 & lightPos, float size, const glm::vec3 & color, const glm::mat4 & view, const glm::mat4 & proj, size_t screenWidth, size_t screenHeight);

	private:
		kengine::EntityManager & _em;
		GodRaysFirstPass & _firstPass;
		size_t _firstPassTextureID;

	public:
		GLint exposure;
		GLint decay;
		GLint density;
		GLint weight;
		GLint lightPositionOnScreen;
		GLint numSamples;
		GLint firstPass;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRays::exposure),
			pmeta_reflectible_attribute(&GodRays::decay),
			pmeta_reflectible_attribute(&GodRays::density),
			pmeta_reflectible_attribute(&GodRays::weight),
			pmeta_reflectible_attribute(&GodRays::lightPositionOnScreen),
			pmeta_reflectible_attribute(&GodRays::numSamples),
			pmeta_reflectible_attribute(&GodRays::firstPass)
		);
	};
}
