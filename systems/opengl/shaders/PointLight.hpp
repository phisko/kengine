#pragma once

#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"
#include "ShadowCubeSrc.hpp"
#include "PointLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::opengl::shaders {
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

	private:
		EntityManager & _em;
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius); 
	};
}

#define refltype kengine::opengl::shaders::PointLight
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::ProjViewModel::Vert::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::ShadowCube::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::PointLight::Frag::Uniforms)
	);
};
#undef refltype
