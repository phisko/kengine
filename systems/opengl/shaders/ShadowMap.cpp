#include "ShadowMap.hpp"
#include "kengine.hpp"

#include "data/InstanceComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/DefaultShadowComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "shaderHelper.hpp"

namespace kengine {
	float SHADOW_MAP_NEAR_PLANE = .1f;
	float SHADOW_MAP_FAR_PLANE = 1000.f;
}

namespace kengine::opengl::shaders {
	ShadowMap::ShadowMap(Entity & parent) noexcept
		: ShadowMapShader(false, putils_nameof(ShadowMap))
	{
		parent += AdjustableComponent{
			"Render/Lights", {
				{ "Shadow map near plane", &SHADOW_MAP_NEAR_PLANE },
				{ "Shadow map far plane", &SHADOW_MAP_FAR_PLANE }
			}
		};
	}

	void ShadowMap::init(size_t firstTextureID) noexcept {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER }
		));

		_proj = glm::mat4(1.f);
	}

	void ShadowMap::drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) noexcept {
		_view = lightSpaceMatrix;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		for (const auto & [e, instance, transform, shadow, noNoShadow] : entities.with<InstanceComponent, TransformComponent, DefaultShadowComponent, no<NoShadowComponent>>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & model = entities.get(instance.model);
			const auto openGL = model.tryGet<SystemSpecificModelComponent<putils::gl::Mesh>>();
			if (!openGL)
				continue;

			_model = matrixHelper::getModelMatrix(model.get<ModelComponent>(), transform);
			shaderHelper::drawModel(*openGL);
		}
	}
}