#include "AssImpShadowCube.hpp"

#include "EntityManager.hpp"

#include "data/SkeletonComponent.hpp"

#include "systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

#include "systems/opengl/shaders/DepthCubeSrc.hpp"
#include "AssImpShaderSrc.hpp"

#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowCube::init(size_t firstTextureID) {
		initWithShaders<AssImpShadowCube>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ Shaders::src::DepthCube::Geom::glsl, GL_GEOMETRY_SHADER },
			ShaderDescription{ Shaders::src::DepthCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_proj = glm::mat4(1.f);
		_view = glm::mat4(1.f);
	}

	void AssImpShadowCube::drawObjects() {
		for (const auto &[e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, TransformComponent, SkeletonComponent>()) {
			AssImpHelper::Uniforms uniforms;
			uniforms.model = _model;
			uniforms.bones = _bones;

			AssImpHelper::drawModel(_em, graphics, transform, skeleton, false, uniforms);
		}
	}
}
