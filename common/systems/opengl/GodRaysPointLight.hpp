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

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const glm::vec3 & camPos, const PointLightComponent & light, const putils::Point3f & pos, const DepthCubeComponent & depthMap, size_t screenWidth, size_t screenHeight);

	private:
		kengine::EntityManager & _em;
		GLuint _shadowMapTextureID;

	public:
		pmeta_get_parents(
			pmeta_reflectible_parent(src::ShadowCube::Frag::Uniforms),
			pmeta_reflectible_parent(src::GodRays::Frag::Uniforms),
			pmeta_reflectible_parent(src::PointLight::GetDirection::Uniforms)
		);
	};
}
