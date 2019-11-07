#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowCube;

	class PointLight : public putils::gl::Program {
	public:
		PointLight(kengine::EntityManager & em)
			: Program(true, pmeta_nameof(PointLight)), _em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<putils::Point3f> _position;

		putils::gl::Uniform<float> _diffuseStrength;
		putils::gl::Uniform<float> _specularStrength;

		putils::gl::Uniform<float> _attenuationConstant;
		putils::gl::Uniform<float> _attenuationLinear;
		putils::gl::Uniform<float> _attenuationQuadratic;

		putils::gl::Uniform<size_t> _shadowMap;
		putils::gl::Uniform<float> _farPlane;
		putils::gl::Uniform<float> _bias;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&PointLight::_proj),
			pmeta_reflectible_attribute_private(&PointLight::_view),
			pmeta_reflectible_attribute_private(&PointLight::_model),

			pmeta_reflectible_attribute_private(&PointLight::_viewPos),
			pmeta_reflectible_attribute_private(&PointLight::_screenSize),

			pmeta_reflectible_attribute_private(&PointLight::_color),
			pmeta_reflectible_attribute_private(&PointLight::_position),

			pmeta_reflectible_attribute_private(&PointLight::_diffuseStrength),
			pmeta_reflectible_attribute_private(&PointLight::_specularStrength),

			pmeta_reflectible_attribute_private(&PointLight::_attenuationConstant),
			pmeta_reflectible_attribute_private(&PointLight::_attenuationLinear),
			pmeta_reflectible_attribute_private(&PointLight::_attenuationQuadratic),

			pmeta_reflectible_attribute_private(&PointLight::_shadowMap),
			pmeta_reflectible_attribute_private(&PointLight::_farPlane),
			pmeta_reflectible_attribute_private(&PointLight::_bias)
		);
	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius); 
	};
}
