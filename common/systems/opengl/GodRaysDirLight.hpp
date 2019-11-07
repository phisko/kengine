#pragma once

#include "opengl/Program.hpp"
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
		GLint SCATTERING;
		GLint NB_STEPS;
		GLint DEFAULT_STEP_LENGTH;
		GLint INTENSITY;

		GLint inverseView;
		GLint inverseProj;
		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint direction;

		GLint shadowMap[KENGINE_CSM_COUNT];
		GLint lightSpaceMatrix[KENGINE_CSM_COUNT];
		GLint cascadeEnd[KENGINE_CSM_COUNT];
		GLint bias;
		GLint pcfSamples;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysDirLight::SCATTERING),
			pmeta_reflectible_attribute(&GodRaysDirLight::NB_STEPS),
			pmeta_reflectible_attribute(&GodRaysDirLight::DEFAULT_STEP_LENGTH),
			pmeta_reflectible_attribute(&GodRaysDirLight::INTENSITY),

			pmeta_reflectible_attribute(&GodRaysDirLight::inverseView),
			pmeta_reflectible_attribute(&GodRaysDirLight::inverseProj),
			pmeta_reflectible_attribute(&GodRaysDirLight::viewPos),
			pmeta_reflectible_attribute(&GodRaysDirLight::screenSize),

			pmeta_reflectible_attribute(&GodRaysDirLight::color),
			pmeta_reflectible_attribute(&GodRaysDirLight::direction),

			pmeta_reflectible_attribute(&GodRaysDirLight::shadowMap),
			pmeta_reflectible_attribute(&GodRaysDirLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&GodRaysDirLight::cascadeEnd),
			pmeta_reflectible_attribute(&GodRaysDirLight::pcfSamples)
		);
	};
}
