#include "ShadowCube.hpp"

#include "data/LightComponent.hpp"
#include "data/DefaultShadowComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "shaderHelper.hpp"
#include "helpers/lightHelper.hpp"

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

	void ShadowCube::drawObjects(const Parameters & params) {
		for (const auto &[e, instance, transform, shadow, noNoShadow] : _em.getEntities<InstanceComponent, TransformComponent, DefaultShadowComponent, no<NoShadowComponent>>()) {
			if (!shaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & model = _em.getEntity(instance.model);
			_model = shaderHelper::getModelMatrix(model.get<ModelComponent>(), transform);
			shaderHelper::drawModel(model.get<OpenGLModelComponent>());
		}
	}
}
