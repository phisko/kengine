#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class SkyBox : public putils::gl::Program {
	public:
		SkyBox(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		EntityManager & _em;
		size_t _textureID;

	public:
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;

		putils::gl::Uniform<size_t> _tex;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&SkyBox::_view),
			putils_reflection_attribute_private(&SkyBox::_proj),

			putils_reflection_attribute_private(&SkyBox::_tex),

			putils_reflection_attribute_private(&SkyBox::_entityID),
			putils_reflection_attribute_private(&SkyBox::_color)
		);
	};
}