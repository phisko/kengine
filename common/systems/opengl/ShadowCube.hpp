#pragma once 

#include "opengl/Program.hpp"
#include "Entity.hpp"

#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowCube : public ShadowCubeShader {
	public:
		ShadowCube(kengine::EntityManager & em) : ShadowCubeShader(false, pmeta_nameof(ShadowCube)), _em(em) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;

		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) override;
		void run(const Parameters & params) override {}

	private:
		kengine::EntityManager & _em;

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