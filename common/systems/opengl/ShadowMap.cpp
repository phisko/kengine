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
	float SHADOW_MAP_MIN_BIAS = .00005f;
	float SHADOW_MAP_MAX_BIAS = .0001f;
}

namespace kengine::Shaders {
	ShadowMap::ShadowMap(kengine::EntityManager & em)
		: ShadowMapShader(false, pmeta_nameof(ShadowMap)), _em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map near plane", &SHADOW_MAP_NEAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map far plane", &SHADOW_MAP_FAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Min shadow map bias", &SHADOW_MAP_MIN_BIAS); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Max shadow map bias", &SHADOW_MAP_MAX_BIAS); };
	}

	void ShadowMap::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
	}

	static void createShadowMap(DepthMapComponent & depthMap) {
		if (depthMap.fbo == -1)
			glGenFramebuffers(1, &depthMap.fbo);
		ShaderHelper::BindFramebuffer __f(depthMap.fbo);

		if (depthMap.texture == -1)
			glGenTextures(1, &depthMap.texture);
		glBindTexture(GL_TEXTURE_2D, depthMap.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)depthMap.size, (GLsizei)depthMap.size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	template<typename T>
	void ShadowMap::runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		if (!e.has<DepthMapComponent>())
			 e.attach<DepthMapComponent>();

		auto & depthMap = e.get<DepthMapComponent>();
		if (depthMap.size != light.shadowMapSize) {
			depthMap.size = light.shadowMapSize;
			createShadowMap(depthMap);
		}

		glViewport(0, 0, light.shadowMapSize, light.shadowMapSize);

		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

		use();
		putils::gl::setUniform(view, LightHelper::getLightSpaceMatrix(light, { pos.x, pos.y, pos.z }, screenWidth, screenHeight));
		glCullFace(GL_FRONT);

		for (const auto &[e, graphics, transform, shadow] : _em.getEntities<GraphicsComponent, TransformComponent3f, DefaultShadowComponent>()) {
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

		glCullFace(GL_BACK);
		glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
	}

	void ShadowMap::run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}

	void ShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
}