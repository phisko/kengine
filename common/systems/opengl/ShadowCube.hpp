#pragma once 

#include "opengl/Program.hpp"
#include "shaders/ProjViewModelSrc.hpp"
#include "shaders/DepthCubeSrc.hpp"
#include "Entity.hpp"

#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowCube : public ShadowCubeShader,
		public src::ProjViewModel::Vert::Uniforms,
		public src::DepthCube::Frag::Uniforms,
		public src::DepthCube::Geom::Uniforms
	{
	public:
		ShadowCube(kengine::EntityManager & em) : ShadowCubeShader(false, putils_nameof(ShadowCube)), _em(em) {}

		void init(size_t firstTextureID) override;

		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) override;
		void run(const Parameters & params) override {}

	private:
		kengine::EntityManager & _em;

	public:
		putils_reflection_parents(
			putils_reflection_parent(src::ProjViewModel::Vert::Uniforms),
			putils_reflection_parent(src::DepthCube::Frag::Uniforms),
			putils_reflection_parent(src::DepthCube::Geom::Uniforms)
		);
	};
}