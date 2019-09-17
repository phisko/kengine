#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Debug : public putils::gl::Program {
	public:
		Debug(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		kengine::EntityManager & _em;

	public:
		GLint model = -1;
		GLint view = -1;
		GLint proj = -1;
		GLint viewPos = -1;

		GLint entityID = -1;
		GLint color = -1;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Debug::model),
			pmeta_reflectible_attribute(&Debug::view),
			pmeta_reflectible_attribute(&Debug::proj),
			pmeta_reflectible_attribute(&Debug::viewPos),

			pmeta_reflectible_attribute(&Debug::entityID),
			pmeta_reflectible_attribute(&Debug::color)
		);

	};
}