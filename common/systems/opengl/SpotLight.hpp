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
		GLint proj = -1;
		GLint view = -1;
		GLint model = -1;

		GLint lightSpaceMatrix = -1;
		GLint shadowMap = -1;
		GLint shadow_map_min_bias = -1;
		GLint shadow_map_max_bias = -1;
		GLint pcfSamples = -1;

		GLint viewPos = -1;
		GLint screenSize = -1;

		GLint color = -1;
		GLint position = -1;
		GLint direction = -1;

		GLint cutOff = -1;
		GLint outerCutOff = -1;

		GLint diffuseStrength = -1;
		GLint specularStrength = -1;

		GLint attenuationConstant = -1;
		GLint attenuationLinear = -1;
		GLint attenuationQuadratic = -1;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpotLight::proj),
			pmeta_reflectible_attribute(&SpotLight::view),
			pmeta_reflectible_attribute(&SpotLight::model),

			pmeta_reflectible_attribute(&SpotLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&SpotLight::shadowMap),
			pmeta_reflectible_attribute(&SpotLight::shadow_map_min_bias),
			pmeta_reflectible_attribute(&SpotLight::shadow_map_max_bias),
			pmeta_reflectible_attribute(&SpotLight::pcfSamples),

			pmeta_reflectible_attribute(&SpotLight::viewPos),
			pmeta_reflectible_attribute(&SpotLight::screenSize),

			pmeta_reflectible_attribute(&SpotLight::color),
			pmeta_reflectible_attribute(&SpotLight::position),
			pmeta_reflectible_attribute(&SpotLight::direction),

			pmeta_reflectible_attribute(&SpotLight::cutOff),
			pmeta_reflectible_attribute(&SpotLight::outerCutOff),

			pmeta_reflectible_attribute(&SpotLight::diffuseStrength),
			pmeta_reflectible_attribute(&SpotLight::specularStrength),

			pmeta_reflectible_attribute(&SpotLight::attenuationConstant),
			pmeta_reflectible_attribute(&SpotLight::attenuationLinear),
			pmeta_reflectible_attribute(&SpotLight::attenuationQuadratic)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID = -1;

		void setLight(const SpotLightComponent & light, const putils::Point3f & pos); 
	};
}