#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class PolyVoxShader : public putils::gl::Program {
	public:
		PolyVoxShader(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		GLint model;
		GLint view;
		GLint proj;
		GLint viewPos;

		GLint entityID;
		GLint color;
		GLint alpha;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PolyVoxShader::model),
			pmeta_reflectible_attribute(&PolyVoxShader::view),
			pmeta_reflectible_attribute(&PolyVoxShader::proj),
			pmeta_reflectible_attribute(&PolyVoxShader::viewPos),

			pmeta_reflectible_attribute(&PolyVoxShader::entityID),
			pmeta_reflectible_attribute(&PolyVoxShader::color),
			pmeta_reflectible_attribute(&PolyVoxShader::alpha)
		);

	private:
		kengine::EntityManager & _em;
	};
}