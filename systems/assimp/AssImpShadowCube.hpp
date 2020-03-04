#pragma once

#include "putils/opengl/Program.hpp"
#include "systems/opengl/shaders/ShadowMapShader.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine {
	class AssImpShadowCube : public Shaders::ShadowCubeShader
	{
	public:
		AssImpShadowCube(EntityManager & em)
			: ShadowCubeShader(false, putils_nameof(AssImpShadowCube)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void drawObjects(const Parameters & params) override;

	private:
		EntityManager & _em;

	public:
		GLint _bones;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&AssImpShadowCube::_bones)
		);

		putils_reflection_parents(
			putils_reflection_type(Shaders::ShadowCubeShader)
		);
	};
}