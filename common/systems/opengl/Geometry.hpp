#pragma once

#include "opengl/Program.hpp"

namespace kengine::Shaders {
	class Geometry : public putils::gl::Program {
	public:
		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	public:
		GLint model;
		GLint view;
		GLint proj;
		GLint viewPos;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Geometry::model),
			pmeta_reflectible_attribute(&Geometry::view),
			pmeta_reflectible_attribute(&Geometry::proj),
			pmeta_reflectible_attribute(&Geometry::viewPos)
		);
	};
}