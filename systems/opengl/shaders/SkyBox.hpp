#pragma once

#include "opengl/Program.hpp"

namespace kengine::opengl::shaders {
	class SkyBox : public putils::gl::Program {
	public:
		SkyBox() noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;

	private:
		size_t _textureID;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;

		putils::gl::Uniform<size_t> _tex;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;
#pragma endregion Uniforms
	};
}

#define refltype kengine::opengl::shaders::SkyBox
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_view),
		putils_reflection_attribute_private(_proj),

		putils_reflection_attribute_private(_tex),

		putils_reflection_attribute_private(_entityID),
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype