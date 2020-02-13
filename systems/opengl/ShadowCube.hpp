#pragma once 

#include "opengl/Program.hpp"
#include "shaders/ProjViewModelSrc.hpp"
#include "shaders/DepthCubeSrc.hpp"
#include "shaders/ShadowMapShader.hpp"
#include "Entity.hpp"

#include "data/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowCube : public ShadowCubeShader
	{
	public:
		ShadowCube(EntityManager & em) : ShadowCubeShader(false, putils_nameof(ShadowCube)), _em(em) {}

		void init(size_t firstTextureID) override;
		void drawObjects() override;

	private:
		EntityManager & _em;

	public:
		putils_reflection_parents(
			putils_reflection_type(ShadowCubeShader)
		);
	};
}