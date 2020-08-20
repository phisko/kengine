#pragma once

#include "opengl/Program.hpp"
#include "opengl/Uniform.hpp"
#include "data/ShaderComponent.hpp"
#include "GodRaysSrc.hpp"
#include "ShadowMapSrc.hpp"
#include "DirLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class GodRaysDirLight : public putils::gl::Program,
		public src::GodRays::Frag::Uniforms,
		public src::CSM::Frag::Uniforms,
		public src::DirLight::GetDirection::Uniforms
	{
	public:
		GodRaysDirLight(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const DirLightComponent & light, const CSMComponent & depthMap, const Parameters & params);

	private:
		EntityManager & _em;
		GLuint _shadowMapTextureID;
	};
}

#define refltype kengine::Shaders::GodRaysDirLight
putils_reflection_info{
	putils_reflection_parents(
		putils_reflection_type(kengine::Shaders::src::GodRays::Frag::Uniforms),
		putils_reflection_type(kengine::Shaders::src::CSM::Frag::Uniforms),
		putils_reflection_type(kengine::Shaders::src::DirLight::GetDirection::Uniforms)
	);
};
#undef refltype
