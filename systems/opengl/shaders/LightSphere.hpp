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

#pragma region Uniforms
	public:
		putils::gl::Uniform<putils::NormalizedColor> _color;

		putils_reflection_parents(
			putils_reflection_type(src::ProjViewModel::Vert::Uniforms)
		);

		putils_reflection_attributes(
			putils_reflection_attribute_private(&LightSphere::_color)
		);
#pragma endregion Uniforms

	private:
		EntityManager & _em;
	};
}