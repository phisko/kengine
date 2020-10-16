#pragma once

#include "systems/opengl/shaders/ShadowMapShader.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShadowMap : public opengl::shaders::ShadowMapShader {
	public:
		AssImpShadowMap(EntityManager & em)
			: ShadowMapShader(false, putils_nameof(AssImpShadowMap)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) override;

	private:
		EntityManager & _em;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		GLint _bones;
#pragma endregion Uniforms
	};
}

#define refltype kengine::AssImpShadowMap
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute_private(_proj),
		putils_reflection_attribute_private(_view),
		putils_reflection_attribute_private(_model),

		putils_reflection_attribute_private(_bones)
	);
};
#undef refltype