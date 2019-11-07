#pragma once

#include "opengl/Program.hpp"
#include "opengl/Uniform.hpp"
#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class GodRaysDirLight : public putils::gl::Program {
	public:
		GodRaysDirLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;
		
	private:
		void drawLight(const DirLightComponent & light, const CSMComponent & depthMap, const Parameters & params);

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
		putils::gl::Uniform<putils::Vector3f> _direction;

		putils::gl::Uniform<size_t> _shadowMap[KENGINE_CSM_COUNT];
		putils::gl::Uniform<glm::mat4> _lightSpaceMatrix[KENGINE_CSM_COUNT];
		putils::gl::Uniform<float> _cascadeEnd[KENGINE_CSM_COUNT];
		putils::gl::Uniform<float> _bias;
		putils::gl::Uniform<float> _pcfSamples;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_scattering),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_nbSteps),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_defaultStepLength),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_intensity),

			pmeta_reflectible_attribute_private(&GodRaysDirLight::_inverseView),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_inverseProj),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_viewPos),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_screenSize),

			pmeta_reflectible_attribute_private(&GodRaysDirLight::_color),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_direction),

			pmeta_reflectible_attribute_private(&GodRaysDirLight::_shadowMap),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_lightSpaceMatrix),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_cascadeEnd),
			pmeta_reflectible_attribute_private(&GodRaysDirLight::_pcfSamples)
		);
	};
}
