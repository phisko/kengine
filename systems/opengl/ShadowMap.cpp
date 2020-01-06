#include "ShadowMap.hpp"
#include "EntityManager.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/DefaultShadowComponent.hpp"

#include "systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

namespace kengine {
	float SHADOW_MAP_NEAR_PLANE = .1f;
	float SHADOW_MAP_FAR_PLANE = 1000.f;
}

namespace kengine::Shaders {
	ShadowMap::ShadowMap(kengine::EntityManager & em)
		: ShadowMapShader(false, putils_nameof(ShadowMap)), _em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map near plane", &SHADOW_MAP_NEAR_PLANE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Lights] Shadow map far plane", &SHADOW_MAP_FAR_PLANE); };
	}

	void ShadowMap::init(size_t firstTextureID) {
		initWithShaders<ShadowMap>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER }
		));

		_proj = glm::mat4(1.f);
	}

	void ShadowMap::drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix) {
		_view = lightSpaceMatrix;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

		for (const auto & [e, graphics, transform, shadow] : _em.getEntities<GraphicsComponent, TransformComponent, DefaultShadowComponent>()) {
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
	}
}