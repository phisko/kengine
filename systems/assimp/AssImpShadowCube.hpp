#pragma once

#include "putils/opengl/Program.hpp"
#include "systems/opengl/shaders/ShadowMapShader.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine {
	class AssImpShadowCube : public opengl::shaders::ShadowCubeShader
	{
	public:
		AssImpShadowCube(EntityManager & em)
			: ShadowCubeShader(false, putils_nameof(AssImpShadowCube)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void drawObjects(const Parameters & params) override;

	private:
		EntityManager & _em;

	public:
		GLint _bones;
	};
}

#define refltype kengine::AssImpShadowCube
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute_private(_bones)
	);

	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::ShadowCubeShader)
	);
};
#undef refltype