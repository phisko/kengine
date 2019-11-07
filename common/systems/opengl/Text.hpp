#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Text : public putils::gl::Program {
	public:
		Text(EntityManager & em);
		~Text();

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
			pmeta_reflectible_attribute_private(&Text::_model),
			pmeta_reflectible_attribute_private(&Text::_view),
			pmeta_reflectible_attribute_private(&Text::_proj),
			pmeta_reflectible_attribute_private(&Text::_viewPos),

			pmeta_reflectible_attribute_private(&Text::_tex),
			pmeta_reflectible_attribute_private(&Text::_color),
			pmeta_reflectible_attribute_private(&Text::_entityID)
		);

	private:
		EntityManager & _em;
		size_t _textureID;
	};
}