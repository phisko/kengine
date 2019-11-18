#pragma once

#include "putils/opengl/Program.hpp"
#include "components/ShaderComponent.hpp"
#include "systems/opengl/shaders/DepthCubeSrc.hpp"

namespace kengine {
	class EntityManager;
	struct PointLightComponent;
}

namespace kengine {
	class AssImpShadowCube : public ShadowCubeShader,
		public Shaders::src::DepthCube::Geom::Uniforms,
		public Shaders::src::DepthCube::Frag::Uniforms
	{
	public:
		AssImpShadowCube(kengine::EntityManager & em)
			: ShadowCubeShader(false, putils_nameof(AssImpShadowCube)),
			_em(em)
		{}

		void init(size_t firstTextureID) override;
		void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) override;
		void run(const Parameters & params) override {}

	private:
		kengine::EntityManager & _em;

	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		GLint _bones;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&AssImpShadowCube::_proj),
			putils_reflection_attribute_private(&AssImpShadowCube::_view),
			putils_reflection_attribute_private(&AssImpShadowCube::_model),

			putils_reflection_attribute_private(&AssImpShadowCube::_bones)
		);

		putils_reflection_parents(
			putils_reflection_parent(Shaders::src::DepthCube::Geom::Uniforms),
			putils_reflection_parent(Shaders::src::DepthCube::Frag::Uniforms)
		);
	};
}