#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShader : public putils::gl::Program {
	public:
		AssImpShader(kengine::EntityManager & em)
			: Program(false, pmeta_nameof(AssImpShader)),
			_em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		GLint model;
		GLint view;
		GLint proj;

		GLint bones;

		GLint hasTexture;
		GLint texture_diffuse;
		GLint texture_specular;
		GLint diffuseColor;
		GLint specularColor;

		GLint entityID;
		GLint color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AssImpShader::model),
			pmeta_reflectible_attribute(&AssImpShader::view),
			pmeta_reflectible_attribute(&AssImpShader::proj),

			pmeta_reflectible_attribute(&AssImpShader::bones),

			pmeta_reflectible_attribute(&AssImpShader::hasTexture),
			pmeta_reflectible_attribute(&AssImpShader::texture_diffuse),
			// pmeta_reflectible_attribute(&TexturedShader::texture_specular),
			pmeta_reflectible_attribute(&AssImpShader::diffuseColor),
			// pmeta_reflectible_attribute(&TexturedShader::specularColor),

			pmeta_reflectible_attribute(&AssImpShader::entityID),
			pmeta_reflectible_attribute(&AssImpShader::color)
		);

	private:
		kengine::EntityManager & _em;
		size_t _diffuseTextureID;
		size_t _specularTextureID;
	};
}