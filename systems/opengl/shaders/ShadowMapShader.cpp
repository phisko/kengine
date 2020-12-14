#include <GL/glew.h>
#include <GL/GL.h>

#include "ShadowMapShader.hpp"
#include "data/LightComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "EntityManager.hpp"

#include "shaderHelper.hpp"
#include "helpers/lightHelper.hpp"

namespace kengine::opengl::shaders {
	template<typename T, typename Func>
	void ShadowMapShader::runImpl(T & depthMap, Func && draw, const Parameters & params) {
		glViewport(0, 0, depthMap.size, depthMap.size);
		glCullFace(GL_FRONT);

		shaderHelper::BindFramebuffer __f(depthMap.fbo);
		shaderHelper::Enable __e(GL_DEPTH_TEST);

		use();

		draw();

		glCullFace(GL_BACK);
		putils::gl::setViewPort(params.viewport);
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

	static void createCSM(CSMComponent & depthMap) {
		depthMap.fbo.generate();
		for (auto & t : depthMap.textures)
			t.generate();

		shaderHelper::BindFramebuffer __f(depthMap.fbo);
		for (const auto & texture : depthMap.textures)
			initTexture(texture, depthMap.size);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.textures[0], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}


	void ShadowMapShader::run(Entity & e, DirLightComponent & light, const Parameters & params) {
		auto depthMap = e.tryGet<CSMComponent>();
		if (!depthMap)
			depthMap = &e.attach<CSMComponent>();

		if (depthMap->size != light.shadowMapSize) {
			depthMap->size = light.shadowMapSize;
			createCSM(*depthMap);
		}

		runImpl(*depthMap, [&] {
			for (size_t i = 0; i < light.cascadeEnds.size(); ++i) {
				const float cascadeStart = (i == 0 ? params.nearPlane : lightHelper::getCSMCascadeEnd(light, i - 1));
				const float cascadeEnd = lightHelper::getCSMCascadeEnd(light, i);
				if (cascadeStart >= cascadeEnd)
					continue;
				drawToTexture(depthMap->textures[i], lightHelper::getCSMLightSpaceMatrix(light, params, i), params);
			}
		}, params);
	}

	static void createShadowMap(DepthMapComponent & depthMap) {
		depthMap.fbo.generate();
		depthMap.texture.generate();

		shaderHelper::BindFramebuffer __f(depthMap.fbo);
		initTexture(depthMap.texture, depthMap.size);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	void ShadowMapShader::run(Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) {
		auto depthMap = e.tryGet<DepthMapComponent>();
		if (!depthMap)
			depthMap = &e.attach<DepthMapComponent>();

		if (depthMap->size != light.shadowMapSize) {
			depthMap->size = light.shadowMapSize;
			createShadowMap(*depthMap);
		}

		runImpl(*depthMap, [&] {
			drawToTexture(depthMap->texture, lightHelper::getLightSpaceMatrix(light, shaderHelper::toVec(pos), params), params);
		}, params);
	}

	void ShadowCubeShader::run(Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) {
		auto depthCube = e.tryGet<DepthCubeComponent>();
		if (!depthCube)
			depthCube = &e.attach<DepthCubeComponent>();

		if (depthCube->size != light.shadowMapSize) {
			depthCube->size = light.shadowMapSize;

			depthCube->fbo.generate();
			shaderHelper::BindFramebuffer __f(depthCube->fbo);

			depthCube->texture.generate();

			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCube->texture);
			for (size_t i = 0; i < 6; ++i)
				glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_DEPTH_COMPONENT, depthCube->size, depthCube->size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCube->texture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		glViewport(0, 0, depthCube->size, depthCube->size);
		glCullFace(GL_FRONT);

		shaderHelper::BindFramebuffer __f(depthCube->fbo);
		shaderHelper::Enable __e(GL_DEPTH_TEST);

		use();

		static struct {
			glm::vec3 target;
			glm::vec3 up;
		} directions[] = {
			{ { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
			{ { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
			{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
			{ { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f } }
		};

		const glm::vec3 vPos(pos.x, pos.y, pos.z);
		const auto proj = glm::perspective(glm::radians(90.f), (float)light.shadowMapSize / (float)light.shadowMapSize, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);
		for (unsigned int i = 0; i < 6; ++i)
			_shadowMatrices[i] = proj * glm::lookAt(vPos, vPos + directions[i].target, directions[i].up);

		_lightPos = pos;
		_farPlane = radius;

		drawObjects(params);

		putils::gl::setViewPort(params.viewport);
		glCullFace(GL_BACK);
	}
}