#pragma once

#include "opengl/Program.hpp"
#include "Entity.hpp"

#include "components/ShaderComponent.hpp"
#include "shaders/ProjViewModelSrc.hpp"

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
		ShadowMap(kengine::EntityManager & em);

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
		putils_reflection_parents(
			putils_reflection_parent(src::ProjViewModel::Vert::Uniforms)
		);
	};

}