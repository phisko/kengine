#pragma once

#include "putils/opengl/Program.hpp"
#include "systems/opengl/shaders/ShadowMapShader.hpp"

namespace kengine {
	struct PointLightComponent;
}

namespace kengine {
	class AssImpShadowCube : public opengl::shaders::ShadowCubeShader
	{
	public:
		AssImpShadowCube() noexcept
			: ShadowCubeShader(false, putils_nameof(AssImpShadowCube))
		{}

		void init(size_t firstTextureID) noexcept override;
		void drawObjects(const Parameters & params) noexcept override;

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