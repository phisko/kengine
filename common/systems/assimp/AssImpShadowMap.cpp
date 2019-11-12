#include "AssImpShadowMap.hpp"
#include "EntityManager.hpp"

#include "components/SkeletonComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "AssImpShaderSrc.hpp"
#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowMap::init(size_t firstTextureID) {
		initWithShaders<AssImpShadowMap>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER }
		));

		_proj = glm::mat4(1.f);
	}

	void AssImpShadowMap::drawToTexture(GLuint texture) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		AssImpHelper::Uniforms uniforms;
		uniforms.model = _model;
		uniforms.bones = _bones;

		for (const auto & [e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, kengine::TransformComponent3f, SkeletonComponent>())
			AssImpHelper::drawModel(_em, graphics, transform, skeleton, false, uniforms);
	}

	template<typename T, typename Func>
	void AssImpShadowMap::runImpl(T & depthMap, Func && draw, const Parameters & params) {
		glViewport(0, 0, depthMap.size, depthMap.size);
		glCullFace(GL_FRONT);

		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		ShaderHelper::Enable depth(GL_DEPTH_TEST);

		use();

		draw();

		glCullFace(GL_BACK);
		putils::gl::setViewPort(params.viewPort);
	}

	void AssImpShadowMap::run(kengine::Entity & e, DirLightComponent & light, const Parameters & params) {
		if (!e.has<CSMComponent>())
			return;

		auto & depthMap = e.get<CSMComponent>();
		if (depthMap.size != light.shadowMapSize)
			return;

		runImpl(depthMap, [&] {
			for (size_t i = 0; i < lengthof(depthMap.textures); ++i) {
				const float cascadeStart = (i == 0 ? params.nearPlane : LightHelper::getCSMCascadeEnd(light, i - 1));
				const float cascadeEnd = LightHelper::getCSMCascadeEnd(light, i);
				if (cascadeStart >= cascadeEnd)
					continue;
				_view = LightHelper::getCSMLightSpaceMatrix(light, params, i);
				drawToTexture(depthMap.textures[i]);
			}
		}, params);
	}

	void AssImpShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) {
		if (!e.has<DepthMapComponent>())
			return;

		auto & depthMap = e.get<DepthMapComponent>();
		if (depthMap.size != light.shadowMapSize)
			return;

		runImpl(depthMap, [&] {
			_view = LightHelper::getLightSpaceMatrix(light, ShaderHelper::toVec(pos), params);
			drawToTexture(depthMap.texture);
		}, params);
	}
}
