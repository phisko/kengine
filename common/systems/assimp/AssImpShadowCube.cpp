#include "AssImpShadowCube.hpp"

#include "EntityManager.hpp"

#include "components/ModelComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

#include "systems/opengl/shaders/DepthCubeSrc.hpp"
#include "AssImpShaderSrc.hpp"

#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowCube::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<AssImpShadowCube>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ kengine::Shaders::src::DepthCube::Geom::glsl, GL_GEOMETRY_SHADER },
			ShaderDescription{ kengine::Shaders::src::DepthCube::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_proj = glm::mat4(1.f);
		_view = glm::mat4(1.f);
	}

	void AssImpShadowCube::run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params) {
		if (!e.has<DepthCubeComponent>())
			return;

		glViewport(0, 0, light.shadowMapSize, light.shadowMapSize);
		glCullFace(GL_FRONT);

		const auto & depthCube = e.get<DepthCubeComponent>();
		ShaderHelper::BindFramebuffer __f(depthCube.fbo);
		ShaderHelper::Enable depth(GL_DEPTH_TEST);

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

		static const float SHADOW_MAP_NEAR_PLANE = .1f;
		static const float SHADOW_MAP_FAR_PLANE = 1000.f;
		const auto proj = glm::perspective(glm::radians(90.f), (float)light.shadowMapSize / (float)light.shadowMapSize, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);
		const glm::vec3 vPos(pos.x, pos.y, pos.z);
		for (unsigned int i = 0; i < 6; ++i)
			_shadowMatrices[i] = proj * glm::lookAt(vPos, vPos + directions[i].target, directions[i].up);

		_lightPos = pos;
		_farPlane = radius;

		for (const auto &[e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, TransformComponent3f, SkeletonComponent>()) {
			AssImpHelper::Uniforms uniforms;
			uniforms.model = _model;
			uniforms.bones = _bones;

			AssImpHelper::drawModel(_em, graphics, transform, skeleton, false, uniforms);
		}

		putils::gl::setViewPort(params.viewPort);
		glCullFace(GL_BACK);
	}
}
