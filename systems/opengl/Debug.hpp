#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Debug : public putils::gl::Program {
	public:
		Debug(kengine::EntityManager & em);

		void init(size_t firstTextureID) override;
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

		putils_reflection_attributes(
			putils_reflection_attribute_private(&Debug::_model),
			putils_reflection_attribute_private(&Debug::_view),
			putils_reflection_attribute_private(&Debug::_proj),
			putils_reflection_attribute_private(&Debug::_viewPos),

			putils_reflection_attribute_private(&Debug::_entityID),
			putils_reflection_attribute_private(&Debug::_color)
		);
	};
}