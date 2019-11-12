#include "ShadowCube.hpp"

#include "components/LightComponent.hpp"
#include "components/DefaultShadowComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/GraphicsComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

namespace kengine::Shaders {
	void ShadowCube::init(size_t firstTextureID) {
		initWithShaders<ShadowCube>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::DepthCube::Geom::glsl, GL_GEOMETRY_SHADER },
			ShaderDescription{ src::DepthCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_proj = glm::mat4(1.f);
		_view = glm::mat4(1.f);
	}

	void ShadowCube::run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) {
		if (!e.has<DepthCubeComponent>())
			e.attach<DepthCubeComponent>();

		auto & depthCube = e.get<DepthCubeComponent>();
		if (depthCube.size != light.shadowMapSize) {
			depthCube.size = light.shadowMapSize;

			if (depthCube.fbo == -1)
				glGenFramebuffers(1, &depthCube.fbo);
			ShaderHelper::BindFramebuffer __f(depthCube.fbo);

			if (depthCube.texture == -1)
				glGenTextures(1, &depthCube.texture);

			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCube.texture);
			for (size_t i = 0; i < 6; ++i)
				glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_DEPTH_COMPONENT, depthCube.size, depthCube.size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCube.texture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		glViewport(0, 0, depthCube.size, depthCube.size);
		glCullFace(GL_FRONT);

		ShaderHelper::BindFramebuffer __f(depthCube.fbo);
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

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

		for (const auto &[e, graphics, transform, shadow] : _em.getEntities<GraphicsComponent, TransformComponent3f, DefaultShadowComponent>()) {
			if (graphics.model == kengine::Entity::INVALID_ID)
				continue;

			const auto & modelInfoEntity = _em.getEntity(graphics.model);
			if (!modelInfoEntity.has<OpenGLModelComponent>() || !modelInfoEntity.has<ModelComponent>())
				continue;

			const auto & modelInfo = modelInfoEntity.get<ModelComponent>();
			const auto & openGL = modelInfoEntity.get<OpenGLModelComponent>();

			_model = ShaderHelper::getModelMatrix(modelInfo, transform);
			ShaderHelper::drawModel(openGL);
		}

		putils::gl::setViewPort(params.viewPort);
		glCullFace(GL_BACK);
	}
}
