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
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		putils::gl::Uniform<putils::NormalizedColor> _color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&LightSphere::_proj),
			pmeta_reflectible_attribute_private(&LightSphere::_view),
			pmeta_reflectible_attribute_private(&LightSphere::_model),
			pmeta_reflectible_attribute_private(&LightSphere::_color)
		);

	private:
		kengine::EntityManager & _em;
	};
}