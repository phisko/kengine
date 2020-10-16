#pragma once 

#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"
#include "DepthCubeSrc.hpp"
#include "ShadowMapShader.hpp"
#include "Entity.hpp"

#include "data/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::opengl::shaders {
	class ShadowCube : public ShadowCubeShader
	{
	public:
		ShadowCube(EntityManager & em) : ShadowCubeShader(false, putils_nameof(ShadowCube)), _em(em) {}

		void init(size_t firstTextureID) override;
		void drawObjects(const Parameters & params) override;

	private:
		EntityManager & _em;
	};
}

#define refltype kengine::opengl::shaders::ShadowCube
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::ShadowCubeShader)
	);
};
#undef refltype