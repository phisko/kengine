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
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		putils::gl::Uniform<putils::Point3f> _lightPos;
		putils::gl::Uniform<float> _farPlane;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&ShadowCube::_proj),
			pmeta_reflectible_attribute_private(&ShadowCube::_view),
			pmeta_reflectible_attribute_private(&ShadowCube::_model),

			pmeta_reflectible_attribute_private(&ShadowCube::_lightPos),
			pmeta_reflectible_attribute_private(&ShadowCube::_farPlane)
		);
	};
}