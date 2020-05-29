#include "AssImpShadowMap.hpp"
#include "EntityManager.hpp"

#include "data/SkeletonComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "helpers/cameraHelper.hpp"

#include "AssImpShaderSrc.hpp"
#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowMap::init(size_t firstTextureID) {
		initWithShaders<AssImpShadowMap>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER }
		));

		_proj = glm::mat4(1.f);
	}

	void AssImpShadowMap::drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) {
		_view = lightSpaceMatrix;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		AssImpHelper::Uniforms uniforms;
		uniforms.model = _model;
		uniforms.bones = _bones;

		for (const auto & [e, textured, instance, transform, skeleton, noNoShadow] : _em.getEntities<AssImpObjectComponent, InstanceComponent, TransformComponent, SkeletonComponent, no<NoShadowComponent>>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;
			AssImpHelper::drawModel(_em, instance, transform, skeleton, false, uniforms);
		}
	}
}
