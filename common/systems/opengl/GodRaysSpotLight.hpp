#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct SpotLightComponent;
	struct DepthMapComponent;
}

namespace kengine::Shaders {
	class GodRaysSpotLight : public putils::gl::Program {
	public:
		GodRaysSpotLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const SpotLightComponent & light, const putils::Point3f & pos, const DepthMapComponent & depthMap, const Parameters & params);

	private:
		kengine::EntityManager & _em;
		GLuint _shadowMapTextureID;

	public:
		putils::gl::Uniform<float> _scattering;
		putils::gl::Uniform<float> _nbSteps;
		putils::gl::Uniform<float> _defaultStepLength;
		putils::gl::Uniform<float> _intensity;

		putils::gl::Uniform<glm::mat4> _inverseView;
		putils::gl::Uniform<glm::mat4> _inverseProj;
		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<putils::Point3f> _position;

		putils::gl::Uniform<size_t> _shadowMap;
		putils::gl::Uniform<glm::mat4> _lightSpaceMatrix;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_scattering),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_nbSteps),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_defaultStepLength),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_intensity),

			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_inverseView),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_inverseProj),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_viewPos),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_screenSize),

			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_color),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_position),

			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_shadowMap),
			pmeta_reflectible_attribute_private(&GodRaysSpotLight::_lightSpaceMatrix)
		);
	};
}
