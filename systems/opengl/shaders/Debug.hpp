#pragma once

#include "opengl/Program.hpp"

namespace kengine::opengl::shaders {
	class Debug : public putils::gl::Program {
	public:
		Debug() noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;
#pragma endregion Uniforms
	};
}

#define refltype kengine::opengl::shaders::Debug
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_model),
		putils_reflection_attribute_private(_view),
		putils_reflection_attribute_private(_proj),
		putils_reflection_attribute_private(_viewPos),

		putils_reflection_attribute_private(_entityID),
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype