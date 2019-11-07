#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
	struct DepthCubeComponent;
}

namespace kengine::Shaders {
	class GodRaysPointLight : public putils::gl::Program {
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
		putils::gl::Uniform<float> _farPlane;

		putils::gl::Uniform<size_t> _shadowMap;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_scattering),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_nbSteps),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_defaultStepLength),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_intensity),

			pmeta_reflectible_attribute_private(&GodRaysPointLight::_inverseView),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_inverseProj),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_viewPos),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_screenSize),

			pmeta_reflectible_attribute_private(&GodRaysPointLight::_color),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_position),
			pmeta_reflectible_attribute_private(&GodRaysPointLight::_farPlane),

			pmeta_reflectible_attribute_private(&GodRaysPointLight::_shadowMap)
		);
	};
}
