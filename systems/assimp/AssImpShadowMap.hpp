#pragma once

#include "systems/opengl/shaders/ShadowMapShader.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShadowMap : public Shaders::ShadowMapShader {
	public:
		AssImpShadowMap(EntityManager & em)
			: ShadowMapShader(false, putils_nameof(AssImpShadowMap)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) override;

	private:
		EntityManager & _em;

	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		GLint _bones;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&AssImpShadowMap::_proj),
			putils_reflection_attribute_private(&AssImpShadowMap::_view),
			putils_reflection_attribute_private(&AssImpShadowMap::_model),

			putils_reflection_attribute_private(&AssImpShadowMap::_bones)
		);
	};
}