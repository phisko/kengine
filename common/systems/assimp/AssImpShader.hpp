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

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;

		GLint _bones;

		putils::gl::Uniform<bool> _hasTexture;
		putils::gl::Uniform<size_t> _texture_diffuse;
		putils::gl::Uniform<size_t> _texture_specular;
		putils::gl::Uniform<putils::NormalizedColor> _diffuseColor;
		putils::gl::Uniform<putils::NormalizedColor> _specularColor;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&AssImpShader::_model),
			pmeta_reflectible_attribute_private(&AssImpShader::_view),
			pmeta_reflectible_attribute_private(&AssImpShader::_proj),

			pmeta_reflectible_attribute_private(&AssImpShader::_bones),

			pmeta_reflectible_attribute_private(&AssImpShader::_hasTexture),
			pmeta_reflectible_attribute_private(&AssImpShader::_texture_diffuse),
			// pmeta_reflectible_attrib_privateute(&TexturedShader::texture_specular),
			pmeta_reflectible_attribute_private(&AssImpShader::_diffuseColor),
			// pmeta_reflectible_attrib_privateute(&TexturedShader::specularColor),

			pmeta_reflectible_attribute_private(&AssImpShader::_entityID),
			pmeta_reflectible_attribute_private(&AssImpShader::_color)
		);

	private:
		kengine::EntityManager & _em;
		size_t _diffuseTextureID;
		size_t _specularTextureID;
	};
}