#pragma once

#include "opengl/Program.hpp"
#include "shaders/ProjViewModelSrc.hpp"
#include "shaders/ShadowCubeSrc.hpp"
#include "shaders/PointLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowCube;

	class PointLight : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms,
		public src::ShadowCube::Frag::Uniforms,
		public src::PointLight::Frag::Uniforms
	{
	public:
		PointLight(EntityManager & em)
			: Program(true, putils_nameof(PointLight)), _em(em)
		{}

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	public:
		putils_reflection_parents(
			putils_reflection_parent(src::ProjViewModel::Vert::Uniforms),
			putils_reflection_parent(src::ShadowCube::Frag::Uniforms),
			putils_reflection_parent(src::PointLight::Frag::Uniforms)
		);

	private:
		EntityManager & _em;
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius); 
	};
}
