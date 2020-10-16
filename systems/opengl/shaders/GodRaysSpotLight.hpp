#pragma once

#include "opengl/Program.hpp"
#include "GodRaysSrc.hpp"
#include "ShadowMapSrc.hpp"
#include "PointLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;

	namespace opengl {
		struct DepthMapComponent;
	}
}

namespace kengine::opengl::shaders {
	class GodRaysSpotLight : public putils::gl::Program,
		public src::GodRays::Frag::Uniforms,
		public src::ShadowMap::Frag::Uniforms,
		public src::PointLight::GetDirection::Uniforms
	{
	public:
		GodRaysSpotLight(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, const Parameters & params);

	private:
		EntityManager & _em;
		GLuint _shadowMapTextureID;
	};
}

#define refltype kengine::opengl::shaders::GodRaysSpotLight
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::GodRays::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::ShadowMap::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::PointLight::GetDirection::Uniforms)
	);
};
#undef refltype
