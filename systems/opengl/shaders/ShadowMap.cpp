#include "ShadowMap.hpp"
#include "EntityManager.hpp"

#include "data/InstanceComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/DefaultShadowComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "shaderHelper.hpp"
#include "helpers/lightHelper.hpp"

namespace kengine {
	float SHADOW_MAP_NEAR_PLANE = .1f;
	float SHADOW_MAP_FAR_PLANE = 1000.f;
}

namespace kengine::Shaders {
	ShadowMap::ShadowMap(EntityManager & em, Entity & parent)
		: ShadowMapShader(false, putils_nameof(ShadowMap)), _em(em)
	{
		parent += AdjustableComponent{
			"Render/Lights", {
				{ "Shadow map near plane", &SHADOW_MAP_NEAR_PLANE },
				{ "Shadow map far plane", &SHADOW_MAP_FAR_PLANE }
			}
		};
	}

	void ShadowMap::init(size_t firstTextureID) {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER }
		));

		_proj = glm::mat4(1.f);
	}

	void ShadowMap::drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) {
		_view = lightSpaceMatrix;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		for (const auto & [e, instance, transform, shadow, noNoShadow] : _em.getEntities<InstanceComponent, TransformComponent, DefaultShadowComponent, no<NoShadowComponent>>()) {
			if (!shaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & model = _em.getEntity(instance.model);
			_model = shaderHelper::getModelMatrix(model.get<ModelComponent>(), transform);
			shaderHelper::drawModel(model.get<OpenGLModelComponent>());
		}
	}
}