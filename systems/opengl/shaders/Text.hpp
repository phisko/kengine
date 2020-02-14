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

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<size_t> _tex;
		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<float> _entityID;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&Text::_model),
			putils_reflection_attribute_private(&Text::_view),
			putils_reflection_attribute_private(&Text::_proj),
			putils_reflection_attribute_private(&Text::_viewPos),

			putils_reflection_attribute_private(&Text::_tex),
			putils_reflection_attribute_private(&Text::_color),
			putils_reflection_attribute_private(&Text::_entityID)
		);

	private:
		EntityManager & _em;
		size_t _textureID;
	};
}