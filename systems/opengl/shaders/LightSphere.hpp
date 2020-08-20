#pragma once

#include <GL/glew.h>
#include "glm/fwd.hpp"
#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"

namespace kengine {
	class EntityManager;
	class Entity;
	struct LightComponent;
}

namespace kengine::Shaders {
	class LightSphere : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms
	{
	public:
		LightSphere(EntityManager & em, Entity & parent);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		void drawLight(const LightComponent & light, const glm::vec3 & pos, float size);

	public:
		putils::gl::Uniform<putils::NormalizedColor> _color;

	private:
		EntityManager & _em;
	};
}

#define refltype kengine::Shaders::LightSphere
putils_reflection_info{
	putils_reflection_parents(
		putils_reflection_type(kengine::Shaders::src::ProjViewModel::Vert::Uniforms)
	);

	putils_reflection_attributes(
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype