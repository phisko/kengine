#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Debug : public putils::gl::Program {
	public:
		Debug(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	private:
		kengine::EntityManager & _em;

	public:
		GLint model;
		GLint view;
		GLint proj;

		GLint entityID;
		GLint color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Debug::model),
			pmeta_reflectible_attribute(&Debug::view),
			pmeta_reflectible_attribute(&Debug::proj),

			pmeta_reflectible_attribute(&Debug::entityID),
			pmeta_reflectible_attribute(&Debug::color)
		);

	};
}