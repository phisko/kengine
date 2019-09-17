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
		GLint model;
		GLint view;
		GLint proj;
		GLint viewPos;

		GLint tex;
		GLint color;
		GLint entityID;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Text::model),
			pmeta_reflectible_attribute(&Text::view),
			pmeta_reflectible_attribute(&Text::proj),
			pmeta_reflectible_attribute(&Text::viewPos),

			pmeta_reflectible_attribute(&Text::tex),
			pmeta_reflectible_attribute(&Text::color),
			pmeta_reflectible_attribute(&Text::entityID)
		);

	private:
		EntityManager & _em;
		size_t _textureID;
	};
}