#pragma once

#include "opengl/Program.hpp"
#include "ShadowMapSrc.hpp"
#include "ProjViewModelSrc.hpp"
#include "SpotLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
}

namespace kengine::opengl::shaders {
	class ShadowMap;

	class SpotLight : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms,
		public src::ShadowMap::Frag::Uniforms,
		public src::SpotLight::Frag::Uniforms
	{
	public:
		SpotLight(EntityManager & em)
			: Program(true, putils_nameof(SpotLight)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		EntityManager & _em;
		size_t _shadowMapTextureID = -1;

		void setLight(const SpotLightComponent & light, const putils::Point3f & pos); 
	};
}

#define refltype kengine::opengl::shaders::SpotLight
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::ProjViewModel::Vert::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::ShadowMap::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::SpotLight::Frag::Uniforms)
	);
};
#undef refltype