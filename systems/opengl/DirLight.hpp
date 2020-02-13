#pragma once

#include "opengl/Program.hpp"
#include "data/ShaderComponent.hpp"
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
		DirLight(EntityManager & em, Entity & parent);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

	private:
		void setLight(const DirLightComponent & light); 

	public:
		putils_reflection_parents(
			putils_reflection_type(src::CSM::Frag::Uniforms),
			putils_reflection_type(src::DirLight::Frag::Uniforms)
		);

	private:
		EntityManager & _em;
		size_t _shadowMapTextureID;
	};
}
