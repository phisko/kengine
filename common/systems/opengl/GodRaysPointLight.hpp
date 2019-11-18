#pragma once

#include "opengl/Program.hpp"
#include "shaders/GodRaysSrc.hpp"
#include "shaders/ShadowCubeSrc.hpp"
#include "shaders/PointLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
	struct DepthCubeComponent;
}

namespace kengine::Shaders {
	class GodRaysPointLight : public putils::gl::Program,
		public src::ShadowCube::Frag::Uniforms,
		public src::GodRays::Frag::Uniforms,
		public src::PointLight::GetDirection::Uniforms
	{
	public:
		GodRaysPointLight(kengine::EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight);

	private:
		kengine::EntityManager & _em;
		GLuint _shadowMapTextureID;

	public:
		putils_reflection_parents(
			putils_reflection_parent(src::ShadowCube::Frag::Uniforms),
			putils_reflection_parent(src::GodRays::Frag::Uniforms),
			putils_reflection_parent(src::PointLight::GetDirection::Uniforms)
		);
	};
}
