#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShader : public putils::gl::Program {
	public:
		AssImpShader(EntityManager & em)
			: Program(false, putils_nameof(AssImpShader)),
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

		putils_reflection_attributes(
			putils_reflection_attribute_private(&AssImpShader::_model),
			putils_reflection_attribute_private(&AssImpShader::_view),
			putils_reflection_attribute_private(&AssImpShader::_proj),

			putils_reflection_attribute_private(&AssImpShader::_bones),

			putils_reflection_attribute_private(&AssImpShader::_hasTexture),
			putils_reflection_attribute_private(&AssImpShader::_texture_diffuse),
			// putils_reflection_attribute_private(&TexturedShader::texture_specular),
			putils_reflection_attribute_private(&AssImpShader::_diffuseColor),
			// putils_reflection_attribute_private(&TexturedShader::specularColor),

			putils_reflection_attribute_private(&AssImpShader::_entityID),
			putils_reflection_attribute_private(&AssImpShader::_color)
		);

	private:
		EntityManager & _em;
		size_t _diffuseTextureID;
		size_t _specularTextureID;
	};
}