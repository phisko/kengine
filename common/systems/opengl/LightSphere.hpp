#pragma once

#include <GL/glew.h>
#include "glm/fwd.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct LightComponent;
}

namespace kengine::Shaders {
	class LightSphere : public putils::gl::Program {
	public:
		LightSphere(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		void drawLight(const LightComponent & light, const glm::vec3 & pos, float size);

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&LightSphere::proj),
			pmeta_reflectible_attribute(&LightSphere::view),
			pmeta_reflectible_attribute(&LightSphere::model),
			pmeta_reflectible_attribute(&LightSphere::color)
		);

	private:
		kengine::EntityManager & _em;
		GLuint _gBufferFBO;
	};
}