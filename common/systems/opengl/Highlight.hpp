#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Highlight : public putils::gl::Program {
	public:
		Highlight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		kengine::EntityManager & _em;

	public:
		GLint viewPos;
		GLint screenSize;

		GLint entityID;
		GLint highlightColor;
		GLint intensity;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Highlight::viewPos),
			pmeta_reflectible_attribute(&Highlight::screenSize),

			pmeta_reflectible_attribute(&Highlight::entityID),
			pmeta_reflectible_attribute(&Highlight::highlightColor),
			pmeta_reflectible_attribute(&Highlight::intensity)
		);
	};
}