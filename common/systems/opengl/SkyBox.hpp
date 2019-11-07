#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class SkyBox : public putils::gl::Program {
	public:
		SkyBox(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		kengine::EntityManager & _em;
		size_t _textureID;

	public:
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;

		putils::gl::Uniform<size_t> _tex;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&SkyBox::_view),
			pmeta_reflectible_attribute_private(&SkyBox::_proj),

			pmeta_reflectible_attribute_private(&SkyBox::_tex),

			pmeta_reflectible_attribute_private(&SkyBox::_entityID),
			pmeta_reflectible_attribute_private(&SkyBox::_color)
		);
	};
}