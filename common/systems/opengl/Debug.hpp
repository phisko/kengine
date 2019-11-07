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
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&Debug::_model),
			pmeta_reflectible_attribute_private(&Debug::_view),
			pmeta_reflectible_attribute_private(&Debug::_proj),
			pmeta_reflectible_attribute_private(&Debug::_viewPos),

			pmeta_reflectible_attribute_private(&Debug::_entityID),
			pmeta_reflectible_attribute_private(&Debug::_color)
		);

	};
}