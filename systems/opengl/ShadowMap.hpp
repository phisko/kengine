#pragma once

#include "opengl/Program.hpp"
#include "Entity.hpp"

#include "data/ShaderComponent.hpp"
#include "shaders/ProjViewModelSrc.hpp"
#include "shaders/ShadowMapShader.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap : public ShadowMapShader,
		public src::ProjViewModel::Vert::Uniforms
	{
	public:
		ShadowMap(EntityManager & em, Entity & parent);

		void init(size_t firstTextureID) override;
		void drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix) override;

	private:
		EntityManager & _em;

	public:
		putils_reflection_parents(
			putils_reflection_parent(src::ProjViewModel::Vert::Uniforms)
		);
	};

}