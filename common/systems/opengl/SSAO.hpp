#pragma once

#include "opengl/Program.hpp"

namespace kengine { class EntityManager; }

namespace kengine::Shaders {
	// Fills a texture that can then be used by SSAOBlur
	class SSAO : public putils::gl::Program {
	public:
		SSAO(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;

		void run(const glm::mat4 & view, const glm::mat4 & projection, size_t screenWidth, size_t screenHeight);
		void run(const Parameters & params) override {}

		auto getTexture() const { return _colorBuffer; }

	public:
		GLint view;
		GLint proj;
		GLint screenSize;

		GLint RADIUS;
		GLint FARCLIP;
		GLint BIAS;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SSAO::view),
			pmeta_reflectible_attribute(&SSAO::proj),
			pmeta_reflectible_attribute(&SSAO::screenSize),

			pmeta_reflectible_attribute(&SSAO::RADIUS),
			pmeta_reflectible_attribute(&SSAO::FARCLIP),
			pmeta_reflectible_attribute(&SSAO::BIAS)
		);

	private:
		GLuint _fbo;
		GLuint _colorBuffer;

	private:
		static float lerp(float a, float b, float f) { return a + f * (b - a); }
	};

	// Fills a texture that can then be used by directional light to know per-pixel AO
	class SSAOBlur : public putils::gl::Program {
	public:
		SSAOBlur() : Program(false, pmeta_nameof(SSAOBlur)) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;

		void run(size_t ssaoTexture, size_t screenWidth, size_t screenHeight);
		void run(const Parameters & params) override {}

		auto getTexture() const { return _colorBuffer; }

	public:
		GLint ssao;
		GLint screenSize;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SSAOBlur::ssao),
			pmeta_reflectible_attribute(&SSAOBlur::screenSize)
		);

	private:
		GLuint _fbo;
		GLuint _colorBuffer;
		GLuint _ssaoTextureID;
	};
}