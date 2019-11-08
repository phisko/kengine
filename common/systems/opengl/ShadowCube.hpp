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
		ShadowCube(kengine::EntityManager & em) : ShadowCubeShader(false, pmeta_nameof(ShadowCube)), _em(em) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;

		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) override;
		void run(const Parameters & params) override {}

	private:
		kengine::EntityManager & _em;

	public:
		pmeta_get_parents(
			pmeta_reflectible_parent(src::ProjViewModel::Vert::Uniforms),
			pmeta_reflectible_parent(src::DepthCube::Frag::Uniforms),
			pmeta_reflectible_parent(src::DepthCube::Geom::Uniforms)
		);
	};
}