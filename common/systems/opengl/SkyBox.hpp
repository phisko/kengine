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
		GLint view;
		GLint proj;

		GLint tex;

		GLint entityID;
		GLint color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SkyBox::view),
			pmeta_reflectible_attribute(&SkyBox::proj),

			pmeta_reflectible_attribute(&SkyBox::tex),

			pmeta_reflectible_attribute(&SkyBox::entityID),
			pmeta_reflectible_attribute(&SkyBox::color)
		);
	};
}