#pragma once

#include "opengl/Program.hpp"
#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class DirLight : public putils::gl::Program {
	public:
		DirLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		void setLight(const DirLightComponent & light); 

	public:
		GLint proj;
		GLint view;

		// shadowMap
		GLint lightSpaceMatrix[KENGINE_CSM_COUNT];
		GLint shadowMap[KENGINE_CSM_COUNT];
		GLint cascadeEnd[KENGINE_CSM_COUNT];
		GLint bias;
		GLint pcfSamples;

		GLint viewPos;
		GLint screenSize;

		GLint color;
		GLint direction;

		GLint ambientStrength;
		GLint diffuseStrength;
		GLint specularStrength;

		GLint debugCSM;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLight::proj),
			pmeta_reflectible_attribute(&DirLight::view),

			pmeta_reflectible_attribute(&DirLight::lightSpaceMatrix),
			pmeta_reflectible_attribute(&DirLight::shadowMap),
			pmeta_reflectible_attribute(&DirLight::cascadeEnd),
			pmeta_reflectible_attribute(&DirLight::bias),
			pmeta_reflectible_attribute(&DirLight::pcfSamples),

			pmeta_reflectible_attribute(&DirLight::viewPos),
			pmeta_reflectible_attribute(&DirLight::screenSize),

			pmeta_reflectible_attribute(&DirLight::color),
			pmeta_reflectible_attribute(&DirLight::direction),

			pmeta_reflectible_attribute(&DirLight::ambientStrength),
			pmeta_reflectible_attribute(&DirLight::diffuseStrength),
			pmeta_reflectible_attribute(&DirLight::specularStrength),

			pmeta_reflectible_attribute(&DirLight::debugCSM)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;
	};
}
