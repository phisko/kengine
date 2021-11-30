#pragma once

#include "opengl/Program.hpp"
#include "data/OpenGLResourceComponent.hpp"
#include "ShadowMapSrc.hpp"
#include "DirLightSrc.hpp"

namespace kengine {
	class Entity;
	struct DirLightComponent;
}

namespace kengine::opengl::shaders {
	class DirLight : public putils::gl::Program,
		public src::CSM::Frag::Uniforms,
		public src::DirLight::Frag::Uniforms
	{
	public:
		DirLight(Entity & parent) noexcept;

		void init(size_t firstTextureID) noexcept override;
		void run(const Parameters & params) noexcept override;

	private:
		void setLight(const DirLightComponent & light) noexcept; 

	private:
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
