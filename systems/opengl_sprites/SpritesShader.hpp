#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class SpritesShader : public putils::gl::Program {
	public:
		SpritesShader(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<size_t> _tex;
		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<float> _entityID;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&SpritesShader::_model),
			putils_reflection_attribute_private(&SpritesShader::_view),
			putils_reflection_attribute_private(&SpritesShader::_proj),
			putils_reflection_attribute_private(&SpritesShader::_viewPos),

			putils_reflection_attribute_private(&SpritesShader::_tex),
			putils_reflection_attribute_private(&SpritesShader::_color),
			putils_reflection_attribute_private(&SpritesShader::_entityID)
		);
#pragma endregion Uniforms

	private:
		EntityManager & _em;
		size_t _textureID;
	};
}