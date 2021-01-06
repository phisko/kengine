#pragma once 

#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"
#include "DepthCubeSrc.hpp"
#include "ShadowMapShader.hpp"
#include "Entity.hpp"

#include "data/ShaderComponent.hpp"

namespace kengine::opengl::shaders {
	class ShadowCube : public ShadowCubeShader
	{
	public:
		ShadowCube() noexcept : ShadowCubeShader(false, putils_nameof(ShadowCube)) {}

		void init(size_t firstTextureID) noexcept override;
		void drawObjects(const Parameters & params) noexcept override;
	};
}

#define refltype kengine::opengl::shaders::ShadowCube
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::ShadowCubeShader)
	);
};
#undef refltype