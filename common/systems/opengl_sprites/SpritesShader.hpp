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
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<size_t> _tex;
		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<float> _entityID;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&SpritesShader::_model),
			pmeta_reflectible_attribute_private(&SpritesShader::_view),
			pmeta_reflectible_attribute_private(&SpritesShader::_proj),
			pmeta_reflectible_attribute_private(&SpritesShader::_viewPos),

			pmeta_reflectible_attribute_private(&SpritesShader::_tex),
			pmeta_reflectible_attribute_private(&SpritesShader::_color),
			pmeta_reflectible_attribute_private(&SpritesShader::_entityID)
		);

	private:
		kengine::EntityManager & _em;
		size_t _textureID;
	};
}