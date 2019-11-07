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
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;

		// shadowMap
		putils::gl::Uniform<glm::mat4> _lightSpaceMatrix[KENGINE_CSM_COUNT];
		putils::gl::Uniform<size_t> _shadowMap[KENGINE_CSM_COUNT];
		putils::gl::Uniform<float> _cascadeEnd[KENGINE_CSM_COUNT];
		putils::gl::Uniform<float> _bias;
		putils::gl::Uniform<int> _pcfSamples;

		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<putils::NormalizedColor> _color;
		putils::gl::Uniform<putils::Vector3f> _direction;

		putils::gl::Uniform<float> _ambientStrength;
		putils::gl::Uniform<float> _diffuseStrength;
		putils::gl::Uniform<float> _specularStrength;

		putils::gl::Uniform<bool> _debugCSM;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&DirLight::_proj),
			pmeta_reflectible_attribute_private(&DirLight::_view),

			pmeta_reflectible_attribute_private(&DirLight::_lightSpaceMatrix),
			pmeta_reflectible_attribute_private(&DirLight::_shadowMap),
			pmeta_reflectible_attribute_private(&DirLight::_cascadeEnd),
			pmeta_reflectible_attribute_private(&DirLight::_bias),
			pmeta_reflectible_attribute_private(&DirLight::_pcfSamples),

			pmeta_reflectible_attribute_private(&DirLight::_viewPos),
			pmeta_reflectible_attribute_private(&DirLight::_screenSize),

			pmeta_reflectible_attribute_private(&DirLight::_color),
			pmeta_reflectible_attribute_private(&DirLight::_direction),

			pmeta_reflectible_attribute_private(&DirLight::_ambientStrength),
			pmeta_reflectible_attribute_private(&DirLight::_diffuseStrength),
			pmeta_reflectible_attribute_private(&DirLight::_specularStrength),

			pmeta_reflectible_attribute_private(&DirLight::_debugCSM)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;
	};
}
