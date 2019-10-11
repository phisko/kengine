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
		GLint proj;
		GLint view;
		GLint model;

		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint position;

		GLint diffuseStrength;
		GLint specularStrength;

		GLint attenuationConstant;
		GLint attenuationLinear;
		GLint attenuationQuadratic;

		GLint shadowMap;
		GLint farPlane;
		GLint bias;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PointLight::proj),
			pmeta_reflectible_attribute(&PointLight::view),
			pmeta_reflectible_attribute(&PointLight::model),

			pmeta_reflectible_attribute(&PointLight::viewPos),
			pmeta_reflectible_attribute(&PointLight::screenSize),

			pmeta_reflectible_attribute(&PointLight::color),
			pmeta_reflectible_attribute(&PointLight::position),

			pmeta_reflectible_attribute(&PointLight::diffuseStrength),
			pmeta_reflectible_attribute(&PointLight::specularStrength),

			pmeta_reflectible_attribute(&PointLight::attenuationConstant),
			pmeta_reflectible_attribute(&PointLight::attenuationLinear),
			pmeta_reflectible_attribute(&PointLight::attenuationQuadratic),

			pmeta_reflectible_attribute(&PointLight::shadowMap),
			pmeta_reflectible_attribute(&PointLight::farPlane),
			pmeta_reflectible_attribute(&PointLight::bias)
		);
	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;

		void setLight(const PointLightComponent & light, const putils::Point3f & pos, float radius); 
	};
}
