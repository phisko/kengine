#pragma once

#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShadowMap : public ShadowMapShader {
	public:
		AssImpShadowMap(kengine::EntityManager & em)
			: ShadowMapShader(false, putils_nameof(AssImpShadowMap)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override {}
		void run(kengine::Entity & e, DirLightComponent & light, const Parameters & params) override;
		void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) override;

	private:
		template<typename T, typename Func>
		void runImpl(T & depthMap, Func && draw, const Parameters & params);
		void drawToTexture(GLuint texture);

		kengine::EntityManager & _em;

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