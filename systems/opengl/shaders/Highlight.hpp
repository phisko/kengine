#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::opengl::shaders {
	class Highlight : public putils::gl::Program {
	public:
		Highlight(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		EntityManager & _em;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _highlightColor;
		putils::gl::Uniform<float> _intensity;
#pragma endregion Uniforms
	};
}

#define refltype kengine::opengl::shaders::Highlight
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_viewPos),
		putils_reflection_attribute_private(_screenSize),

		putils_reflection_attribute_private(_entityID),
		putils_reflection_attribute_private(_highlightColor),
		putils_reflection_attribute_private(_intensity)
	);
};
#undef refltype