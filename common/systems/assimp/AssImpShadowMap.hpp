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

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override {}
		void run(kengine::Entity & e, DirLightComponent & light, const Parameters & params) override;
		void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) override;

	private:
		template<typename T, typename Func>
		void runImpl(T & depthMap, Func && draw, const Parameters & params);
		void drawToTexture(GLuint texture);

		kengine::EntityManager & _em;

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint bones;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AssImpShadowMap::proj),
			pmeta_reflectible_attribute(&AssImpShadowMap::view),
			pmeta_reflectible_attribute(&AssImpShadowMap::model),

			pmeta_reflectible_attribute(&AssImpShadowMap::bones)
		);
	};
}