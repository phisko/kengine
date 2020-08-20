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

	private:
		EntityManager & _em;
		size_t _textureID;
	};
}

#define refltype kengine::Shaders::Text
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_model),
		putils_reflection_attribute_private(_view),
		putils_reflection_attribute_private(_proj),
		putils_reflection_attribute_private(_viewPos),

		putils_reflection_attribute_private(_tex),
		putils_reflection_attribute_private(_color),
		putils_reflection_attribute_private(_entityID)
	);
};
#undef refltype