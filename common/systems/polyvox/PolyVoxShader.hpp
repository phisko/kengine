#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class PolyVoxShader : public putils::gl::Program {
	public:
		PolyVoxShader(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight);

	public:
		GLint model;
		GLint view;
		GLint proj;
		GLint viewPos;

		GLint entityID;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PolyVoxShader::model),
			pmeta_reflectible_attribute(&PolyVoxShader::view),
			pmeta_reflectible_attribute(&PolyVoxShader::proj),
			pmeta_reflectible_attribute(&PolyVoxShader::viewPos),

			pmeta_reflectible_attribute(&PolyVoxShader::entityID)
		);

	private:
		kengine::EntityManager & _em;
	};
}