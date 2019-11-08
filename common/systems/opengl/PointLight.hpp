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
		PointLight(kengine::EntityManager & em)
			: Program(true, pmeta_nameof(PointLight)), _em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		pmeta_get_parents(
			pmeta_reflectible_parent(src::ProjViewModel::Vert::Uniforms),
			pmeta_reflectible_parent(src::ShadowCube::Frag::Uniforms),
			pmeta_reflectible_parent(src::PointLight::Frag::Uniforms)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius); 
	};
}
