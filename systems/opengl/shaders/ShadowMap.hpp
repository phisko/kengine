#pragma once

#include "opengl/Program.hpp"
#include "Entity.hpp"

#include "data/ShaderComponent.hpp"
#include "ProjViewModelSrc.hpp"
#include "ShadowMapShader.hpp"

namespace kengine::opengl::shaders {
	class ShadowMap : public ShadowMapShader,
		public src::ProjViewModel::Vert::Uniforms
	{
	public:
		ShadowMap(Entity & parent) noexcept;

		void init(size_t firstTextureID) noexcept override;
		void drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) noexcept override;
	};

}

#define refltype kengine::opengl::shaders::ShadowMap
putils_reflection_info {
	putils_reflection_parents(
		putils_reflection_type(kengine::opengl::shaders::src::ProjViewModel::Vert::Uniforms)
	);
};
#undef refltype