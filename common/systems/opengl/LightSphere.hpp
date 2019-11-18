#pragma once

#include <GL/glew.h>
#include "glm/fwd.hpp"
#include "opengl/Program.hpp"
#include "shaders/ProjViewModelSrc.hpp"

namespace kengine {
	class EntityManager;
	struct LightComponent;
}

namespace kengine::Shaders {
	class LightSphere : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms
	{
	public:
		LightSphere(kengine::EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		void drawLight(const LightComponent & light, const glm::vec3 & pos, float size);

	public:
		putils::gl::Uniform<putils::NormalizedColor> _color;

	public:
		putils_reflection_parents(
			putils_reflection_parent(src::ProjViewModel::Vert::Uniforms)
		);

		putils_reflection_attributes(
			putils_reflection_attribute_private(&LightSphere::_color)
		);

	private:
		kengine::EntityManager & _em;
	};
}