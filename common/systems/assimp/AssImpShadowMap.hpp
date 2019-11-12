#pragma once

#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;

	class AssImpShadowMap : public ShadowMapShader {
	public:
		AssImpShadowMap(kengine::EntityManager & em)
			: ShadowMapShader(false, pmeta_nameof(AssImpShadowMap)),
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

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&AssImpShadowMap::_proj),
			pmeta_reflectible_attribute_private(&AssImpShadowMap::_view),
			pmeta_reflectible_attribute_private(&AssImpShadowMap::_model),

			pmeta_reflectible_attribute_private(&AssImpShadowMap::_bones)
		);
	};
}