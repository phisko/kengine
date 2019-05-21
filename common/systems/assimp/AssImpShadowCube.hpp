#pragma once

#include "putils/opengl/Program.hpp"
#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine {
	class AssImpShadowCube : public ShadowCubeShader {
	public:
		AssImpShadowCube(kengine::EntityManager & em)
			: ShadowCubeShader(false, pmeta_nameof(AssImpShadowCube)),
			_em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, size_t screenWidth, size_t screenHeight);
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {}

	private:
		kengine::EntityManager & _em;

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint lightPos;
		GLint farPlane;

		GLint bones;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AssImpShadowCube::proj),
			pmeta_reflectible_attribute(&AssImpShadowCube::view),
			pmeta_reflectible_attribute(&AssImpShadowCube::model),

			pmeta_reflectible_attribute(&AssImpShadowCube::lightPos),
			pmeta_reflectible_attribute(&AssImpShadowCube::farPlane),

			pmeta_reflectible_attribute(&AssImpShadowCube::bones)
		);

	};
}