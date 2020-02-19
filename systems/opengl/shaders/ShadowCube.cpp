#include "ShadowCube.hpp"

#include "data/LightComponent.hpp"
#include "data/DefaultShadowComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "ShaderHelper.hpp"
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

	void ShadowCube::drawObjects() {
		for (const auto &[e, graphics, transform, shadow, noNoShadow] : _em.getEntities<GraphicsComponent, TransformComponent, DefaultShadowComponent, no<NoShadowComponent>>()) {
			if (graphics.model == Entity::INVALID_ID)
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
