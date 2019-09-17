#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class SpritesShader : public putils::gl::Program {
	public:
		SpritesShader(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		GLint model;
		GLint view;
		GLint proj;
		GLint viewPos;

		GLint tex;
		GLint color;
		GLint entityID;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpritesShader::model),
			pmeta_reflectible_attribute(&SpritesShader::view),
			pmeta_reflectible_attribute(&SpritesShader::proj),
			pmeta_reflectible_attribute(&SpritesShader::viewPos),

			pmeta_reflectible_attribute(&SpritesShader::tex),
			pmeta_reflectible_attribute(&SpritesShader::color),
			pmeta_reflectible_attribute(&SpritesShader::entityID)
		);

	private:
		kengine::EntityManager & _em;
		size_t _textureID;
	};
}