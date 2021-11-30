#pragma once

#include "opengl/Program.hpp"
#include "ProjViewModelSrc.hpp"
#include "ShadowCubeSrc.hpp"
#include "PointLightSrc.hpp"

namespace kengine {
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
		PointLight() noexcept
			: Program(true, putils_nameof(PointLight))
		{}

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;

	private:
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius) noexcept; 
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
