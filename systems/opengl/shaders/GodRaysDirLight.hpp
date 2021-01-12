#pragma once

#include "opengl/Program.hpp"
#include "opengl/Uniform.hpp"
#include "data/OpenGLResourceComponent.hpp"
#include "GodRaysSrc.hpp"
#include "ShadowMapSrc.hpp"
#include "DirLightSrc.hpp"

namespace kengine {
	struct DirLightComponent;
}

namespace kengine::opengl::shaders {
	class GodRaysDirLight : public putils::gl::Program,
		public src::GodRays::Frag::Uniforms,
		public src::CSM::Frag::Uniforms,
		public src::DirLight::GetDirection::Uniforms
	{
	public:
		GodRaysDirLight() noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;
		
	private:
		void drawLight(const DirLightComponent & light, const opengl::CSMComponent & depthMap, const Parameters & params) noexcept;

	private:
		GLuint _shadowMapTextureID;
	};
}

#define refltype kengine::opengl::shaders::GodRaysDirLight
putils_reflection_info{
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::GodRays::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::CSM::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::DirLight::GetDirection::Uniforms)
	);
};
#undef refltype
