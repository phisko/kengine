#include "AssImpShadowCube.hpp"

#include "kengine.hpp"

#include "data/SkeletonComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "helpers/cameraHelper.hpp"

#include "systems/opengl/shaders/DepthCubeSrc.hpp"
#include "AssImpShaderSrc.hpp"

#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowCube::init(size_t firstTextureID) noexcept {
		initWithShaders<AssImpShadowCube>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ opengl::shaders::src::DepthCube::Geom::glsl, GL_GEOMETRY_SHADER },
			ShaderDescription{ opengl::shaders::src::DepthCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_proj = glm::mat4(1.f);
		_view = glm::mat4(1.f);
	}

	void AssImpShadowCube::drawObjects(const Parameters & params) noexcept {
		for (const auto &[e, textured, instance, transform, skeleton, noNoShadow] : entities.with<AssImpObjectComponent, InstanceComponent, TransformComponent, SkeletonComponent, no<NoShadowComponent>>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;
			AssImpHelper::Uniforms uniforms;
			uniforms.model = _model;
			uniforms.bones = _bones;

			AssImpHelper::drawModel(instance, transform, skeleton, false, uniforms);
		}
	}
}
