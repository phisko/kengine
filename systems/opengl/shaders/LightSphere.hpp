#pragma once

#include <GL/glew.h>
#include "glm/fwd.hpp"
#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"

namespace kengine {
	class Entity;
	struct LightComponent;
}

namespace kengine::opengl::shaders {
	class LightSphere : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms
	{
	public:
		LightSphere(Entity & parent) noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;

	private:
		void drawLight(const LightComponent & light, const glm::vec3 & pos, float size) noexcept;

	public:
		putils::gl::Uniform<putils::NormalizedColor> _color;
	};
}

#define refltype kengine::opengl::shaders::LightSphere
putils_reflection_info{
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::ProjViewModel::Vert::Uniforms)
	);

	putils_reflection_attributes(
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype