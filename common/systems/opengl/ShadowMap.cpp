#include "ShadowMap.hpp"
#include "RAII.hpp"
#include "EntityManager.hpp"
#include "LightHelper.hpp"
#include "shaders/shaders.hpp"

#include "components/AdjustableComponent.hpp"
#include "components/LightComponent.hpp"

namespace kengine {
	float SHADOW_MAP_NEAR_PLANE = 0.f;
	float SHADOW_MAP_FAR_PLANE = 1000.f;
	float DIRECTIONAL_LIGHT_SHADOW_DISTANCE = 200.f;
}

namespace kengine::Shaders {
	ShadowMap::ShadowMap(kengine::EntityManager & em)
		: Program(false, pmeta_nameof(ShadowMap))
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map near plane", &SHADOW_MAP_NEAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map far plane", &SHADOW_MAP_FAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Directional light shadow distance", &DIRECTIONAL_LIGHT_SHADOW_DISTANCE); };
	}

	void ShadowMap::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
	}

	static void createShadowMap(GLuint & depthMapFBO, GLuint & depthMapTexture, size_t width, size_t height) {
		glGenFramebuffers(1, &depthMapFBO);
		BindFramebuffer __f(depthMapFBO);

		glGenTextures(1, &depthMapTexture);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	template<typename T>
	void ShadowMap::runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		static constexpr auto SHADOW_MAP_SIZE = 8192;

		if (!e.has<DepthMapComponent>()) {
			auto & depthMap = e.attach<DepthMapComponent>();
			createShadowMap(depthMap.fbo, depthMap.texture, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
		}

		glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

		const auto & depthMap = e.get<DepthMapComponent>();
		BindFramebuffer __f(depthMap.fbo);
		Enable __e(GL_DEPTH_TEST);

		use();
		glClear(GL_DEPTH_BUFFER_BIT);
		putils::gl::setUniform(view, LightHelper::getLightSpaceMatrix(light, { pos.x, pos.y, pos.z }, screenWidth, screenHeight));
		glCullFace(GL_FRONT);
		drawObjects(model);
		glCullFace(GL_BACK);
		glViewport(0, 0, screenWidth, screenHeight);
	}

	void ShadowMap::run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}

	void ShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
}