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
		void run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) override;
		void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) override;

	private:
		template<typename T>
		void runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight);

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