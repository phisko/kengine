#include "ShadowMap.hpp"
#include "EntityManager.hpp"
#include "shaders/shaders.hpp"

#include "components/GraphicsComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/DefaultShadowComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

namespace kengine {
	float SHADOW_MAP_NEAR_PLANE = .1f;
	float SHADOW_MAP_FAR_PLANE = 1000.f;
}

namespace kengine::Shaders {
	ShadowMap::ShadowMap(kengine::EntityManager & em)
		: ShadowMapShader(false, pmeta_nameof(ShadowMap)), _em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map near plane", &SHADOW_MAP_NEAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map far plane", &SHADOW_MAP_FAR_PLANE); };
	}

	void ShadowMap::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
	}

	static void initTexture(GLuint texture, size_t size) {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)size, (GLsizei)size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	static void createShadowMap(DepthMapComponent & depthMap) {
		if (depthMap.fbo == -1) {
			glGenFramebuffers(1, &depthMap.fbo);
			glGenTextures(1, &depthMap.texture);
		}

		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		initTexture(depthMap.texture, depthMap.size);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	static void createCSM(CSMComponent & depthMap) {
		if (depthMap.fbo == -1) {
			glGenFramebuffers(1, &depthMap.fbo);
			glGenTextures(lengthof(depthMap.textures), depthMap.textures);
		}

		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		for (const auto texture : depthMap.textures)
			initTexture(texture, depthMap.size);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.textures[0], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	void ShadowMap::drawToTexture(GLuint texture) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		for (const auto & [e, graphics, transform, shadow] : _em.getEntities<GraphicsComponent, TransformComponent3f, DefaultShadowComponent>()) {
			if (graphics.model == kengine::Entity::INVALID_ID)
				continue;

			const auto & modelInfoEntity = _em.getEntity(graphics.model);
			if (!modelInfoEntity.has<OpenGLModelComponent>() || !modelInfoEntity.has<ModelComponent>())
				continue;

			const auto & modelInfo = modelInfoEntity.get<ModelComponent>();
			const auto & openGL = modelInfoEntity.get<OpenGLModelComponent>();

			putils::gl::setUniform(this->model, ShaderHelper::getModelMatrix(modelInfo, transform));
			ShaderHelper::drawModel(openGL);
		}
	}

	template<typename T, typename Func>
	void ShadowMap::runImpl(T & depthMap, Func && draw, const Parameters & params) {
		glViewport(0, 0, depthMap.size, depthMap.size);
		glCullFace(GL_FRONT);

		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

		use();

		draw();

		glCullFace(GL_BACK);
		putils::gl::setViewPort(params.viewPort);
	}

	void ShadowMap::run(kengine::Entity & e, DirLightComponent & light, const Parameters & params) {
		if (!e.has<CSMComponent>())
			 e.attach<CSMComponent>();

		auto & depthMap = e.get<CSMComponent>();
		if (depthMap.size != light.shadowMapSize) {
			depthMap.size = light.shadowMapSize;
			createCSM(depthMap);
		}

		runImpl(depthMap, [&] {
			for (size_t i = 0; i < lengthof(depthMap.textures); ++i) {
				const float cascadeStart = (i == 0 ? params.nearPlane : LightHelper::getCSMCascadeEnd(light, i - 1));
				const float cascadeEnd = LightHelper::getCSMCascadeEnd(light, i);
				if (cascadeStart >= cascadeEnd)
					continue;
				putils::gl::setUniform(view, LightHelper::getCSMLightSpaceMatrix(light, params, i));
				drawToTexture(depthMap.textures[i]);
			}
		}, params);
	}

	void ShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) {
		if (!e.has<DepthMapComponent>())
			e.attach<DepthMapComponent>();

		auto & depthMap = e.get<DepthMapComponent>();
		if (depthMap.size != light.shadowMapSize) {
			depthMap.size = light.shadowMapSize;
			createShadowMap(depthMap);
		}

		runImpl(depthMap, [&] {
			putils::gl::setUniform(view, LightHelper::getLightSpaceMatrix(light, ShaderHelper::toVec(pos), params));
			drawToTexture(depthMap.texture);
		}, params);
	}
}