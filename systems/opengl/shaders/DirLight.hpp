#pragma once

#include "opengl/Program.hpp"
#include "data/ShaderComponent.hpp"
#include "ShadowMapSrc.hpp"
#include "DirLightSrc.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
}

namespace kengine::opengl::shaders {
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

	private:
		EntityManager & _em;
		size_t _shadowMapTextureID;
	};
}

#define refltype kengine::opengl::shaders::DirLight
putils_reflection_info{
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::CSM::Frag::Uniforms),
		putils_reflection_type(kengine::opengl::shaders::src::DirLight::Frag::Uniforms)
	);
};
#undef refltype
