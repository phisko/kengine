#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap;

	class SpotLight : public putils::gl::Program {
	public:
		SpotLight(kengine::EntityManager & em)
			: Program(true, pmeta_nameof(SpotLight)),
			_em(em)
		{}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		putils::gl::Uniform<glm::mat4> _lightSpaceMatrix;
		putils::gl::Uniform<size_t> _shadowMap;
		putils::gl::Uniform<float> _bias;
		putils::gl::Uniform<int> _pcfSamples;

		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<putils::Point3f> _position;
		putils::gl::Uniform<putils::Vector3f> _direction;

		putils::gl::Uniform<float> _cutOff;
		putils::gl::Uniform<float> _outerCutOff;

		putils::gl::Uniform<float> _diffuseStrength;
		putils::gl::Uniform<float> _specularStrength;

		putils::gl::Uniform<float> _attenuationConstant;
		putils::gl::Uniform<float> _attenuationLinear;
		putils::gl::Uniform<float> _attenuationQuadratic;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&SpotLight::_proj),
			pmeta_reflectible_attribute_private(&SpotLight::_view),
			pmeta_reflectible_attribute_private(&SpotLight::_model),

			pmeta_reflectible_attribute_private(&SpotLight::_lightSpaceMatrix),
			pmeta_reflectible_attribute_private(&SpotLight::_shadowMap),
			pmeta_reflectible_attribute_private(&SpotLight::_bias),
			pmeta_reflectible_attribute_private(&SpotLight::_pcfSamples),

			pmeta_reflectible_attribute_private(&SpotLight::_viewPos),
			pmeta_reflectible_attribute_private(&SpotLight::_screenSize),

			pmeta_reflectible_attribute_private(&SpotLight::_color),
			pmeta_reflectible_attribute_private(&SpotLight::_position),
			pmeta_reflectible_attribute_private(&SpotLight::_direction),

			pmeta_reflectible_attribute_private(&SpotLight::_cutOff),
			pmeta_reflectible_attribute_private(&SpotLight::_outerCutOff),

			pmeta_reflectible_attribute_private(&SpotLight::_diffuseStrength),
			pmeta_reflectible_attribute_private(&SpotLight::_specularStrength),

			pmeta_reflectible_attribute_private(&SpotLight::_attenuationConstant),
			pmeta_reflectible_attribute_private(&SpotLight::_attenuationLinear),
			pmeta_reflectible_attribute_private(&SpotLight::_attenuationQuadratic)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID = -1;

		void setLight(const SpotLightComponent & light, const putils::Point3f & pos); 
	};
}