#pragma once

#include "opengl/Program.hpp"
#include "GodRaysSrc.hpp"
#include "ShadowCubeSrc.hpp"
#include "PointLightSrc.hpp"

namespace kengine {
	struct PointLightComponent;

	namespace opengl {
		struct DepthCubeComponent;
	}
}

namespace kengine::opengl::shaders {
	class GodRaysPointLight : public putils::gl::Program,
		public src::ShadowCube::Frag::Uniforms,
		public src::GodRays::Frag::Uniforms,
		public src::PointLight::GetDirection::Uniforms
	{
	public:
		GodRaysPointLight() noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight) noexcept;

	private:
		GLuint _shadowMapTextureID;
	};
}

#define refltype kengine::opengl::shaders::GodRaysPointLight
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::ShadowCube::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::GodRays::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::PointLight::GetDirection::Uniforms)
	);
};
#undef refltype
