#pragma once

#include "opengl/Program.hpp"
#include "components/ShaderComponent.hpp"
#include "shaders/ShadowMapSrc.hpp"
#include "shaders/DirLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::Shaders {
	class DirLight : public putils::gl::Program,
		public src::CSM::Frag::Uniforms,
		public src::DirLight::Frag::Uniforms
	{
	public:
		DirLight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		void setLight(const DirLightComponent & light); 

	public:
		pmeta_get_parents(
			pmeta_reflectible_parent(src::CSM::Frag::Uniforms),
			pmeta_reflectible_parent(src::DirLight::Frag::Uniforms)
		);

	private:
		kengine::EntityManager & _em;
		size_t _shadowMapTextureID;
	};
}
