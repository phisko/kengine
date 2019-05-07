#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class TexturedShader : public putils::gl::Program {
	public:
		TexturedShader(kengine::EntityManager & em)
			: Program(false, pmeta_nameof(TexturedShader)),
			_em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override;

	public:
		GLint model;
		GLint view;
		GLint proj;

		GLint bones;

		GLint hasTexture;
		GLint texture_diffuse;
		GLint texture_specular;

		GLint entityID;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&TexturedShader::model),
			pmeta_reflectible_attribute(&TexturedShader::view),
			pmeta_reflectible_attribute(&TexturedShader::proj),

			pmeta_reflectible_attribute(&TexturedShader::bones),

			pmeta_reflectible_attribute(&TexturedShader::hasTexture),
			pmeta_reflectible_attribute(&TexturedShader::texture_diffuse),
			pmeta_reflectible_attribute(&TexturedShader::texture_specular),

			pmeta_reflectible_attribute(&TexturedShader::entityID)
		);

	private:
		kengine::EntityManager & _em;
		size_t _diffuseTextureID;
		size_t _specularTextureID;
	};
}