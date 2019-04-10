#pragma once 

#include "opengl/Program.hpp"
#include "Entity.hpp"

namespace kengine { struct PointLightComponent; }
namespace kengine::Shaders {
	struct DepthCubeComponent {
		GLuint fbo = -1;
		GLuint texture;
		pmeta_get_class_name(DepthCubeComponent);
	};

	class ShadowCube : public putils::gl::Program {
	public:
		ShadowCube() : Program(false, pmeta_nameof(ShadowCube)) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;

		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, size_t screenWidth, size_t screenHeight);
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {}

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint lightPos;
		GLint farPlane;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ShadowCube::proj),
			pmeta_reflectible_attribute(&ShadowCube::view),
			pmeta_reflectible_attribute(&ShadowCube::model),

			pmeta_reflectible_attribute(&ShadowCube::lightPos),
			pmeta_reflectible_attribute(&ShadowCube::farPlane)
		);
	};
}