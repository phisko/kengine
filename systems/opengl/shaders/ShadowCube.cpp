#include "ShadowCube.hpp"

#include "data/LightComponent.hpp"
#include "data/DefaultShadowComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/NoShadowComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/lightHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "shaderHelper.hpp"

namespace kengine::opengl::shaders {
	void ShadowCube::init(size_t firstTextureID) noexcept {
		initWithShaders<ShadowCube>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ src::DepthCube::Geom::glsl, GL_GEOMETRY_SHADER },
			ShaderDescription{ src::DepthCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_proj = glm::mat4(1.f);
		_view = glm::mat4(1.f);
	}

	void ShadowCube::drawObjects(const Parameters & params) noexcept {
		for (const auto &[e, instance, transform, shadow, noNoShadow] : entities.with<InstanceComponent, TransformComponent, DefaultShadowComponent, no<NoShadowComponent>>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto & model = entities[instance.model];
			const auto openGL = model.tryGet<SystemSpecificModelComponent<putils::gl::Mesh>>();
			if (!openGL)
				continue;

			_model = matrixHelper::getModelMatrix(transform, model.tryGet<TransformComponent>());
			shaderHelper::drawModel(*openGL);
		}
	}
}
