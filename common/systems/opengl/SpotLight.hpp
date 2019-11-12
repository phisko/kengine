#pragma once

#include "opengl/Program.hpp"
#include "shaders/ShadowMapSrc.hpp"
#include "shaders/ProjViewModelSrc.hpp"
#include "shaders/SpotLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap;

	class SpotLight : public putils::gl::Program,
		public src::ProjViewModel::Vert::Uniforms,
		public src::ShadowMap::Frag::Uniforms,
		public src::SpotLight::Frag::Uniforms
	{
	public:
		SpotLight(kengine::EntityManager & em)
			: Program(true, pmeta_nameof(SpotLight)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	public:
		pmeta_get_parents(
			pmeta_reflectible_parent(src::ProjViewModel::Vert::Uniforms),
			pmeta_reflectible_parent(src::ShadowMap::Frag::Uniforms),
			pmeta_reflectible_parent(src::SpotLight::Frag::Uniforms)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID = -1;

		void setLight(const SpotLightComponent & light, const putils::Point3f & pos); 
	};
}