#include "AssImpShadowCube.hpp"

#include "EntityManager.hpp"

#include "components/ModelComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

#include "systems/opengl/shaders/shaders.hpp"
#include "AssImpShaderSrc.hpp"

#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowCube::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<AssImpShadowCube>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ kengine::Shaders::src::DepthCube::geom, GL_GEOMETRY_SHADER },
			ShaderDescription{ kengine::Shaders::src::DepthCube::frag, GL_FRAGMENT_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
		putils::gl::setUniform(view, glm::mat4(1.f));
	}

	void AssImpShadowCube::run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, size_t screenWidth, size_t screenHeight) {
		if (!e.has<DepthCubeComponent>())
			return;

		glViewport(0, 0, KENGINE_SHADOW_CUBE_SIZE, KENGINE_SHADOW_CUBE_SIZE);
		glCullFace(GL_FRONT);

		const auto & depthCube = e.get<DepthCubeComponent>();
		ShaderHelper::BindFramebuffer __f(depthCube.fbo);
		ShaderHelper::Enable depth(GL_DEPTH_TEST);

		use();

		static struct {
			GLint shadowMatrixUniform;
			glm::vec3 target;
			glm::vec3 up;
		} directions[] = {
			{ -1, { 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
			{ -1, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
			{ -1, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ -1, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ -1, { 0.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
			{ -1, { 0.0f, 0.0f, -1.0f }, { 0.0f, -1.0f, 0.0f } }
		};

		const glm::vec3 vPos(pos.x, pos.y, pos.z);
		static const float SHADOW_MAP_NEAR_PLANE = .1f;
		static const float SHADOW_MAP_FAR_PLANE = 1000.f;
		const auto proj = glm::perspective(glm::radians(90.f), (float)KENGINE_SHADOW_CUBE_SIZE / (float)KENGINE_SHADOW_CUBE_SIZE, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);
		for (unsigned int i = 0; i < 6; ++i) {
			if (directions[i].shadowMatrixUniform == -1) {
				const putils::string<64> shadowMatrix("shadowMatrices[%d]", i);
				directions[i].shadowMatrixUniform = glGetUniformLocation(getHandle(), shadowMatrix.c_str());
				assert(directions[i].shadowMatrixUniform != -1);
			}
			putils::gl::setUniform(directions[i].shadowMatrixUniform, proj * glm::lookAt(vPos, vPos + directions[i].target, directions[i].up));
		}

		putils::gl::setUniform(lightPos, vPos);
		putils::gl::setUniform(farPlane, radius);

		for (const auto &[e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, TransformComponent3f, SkeletonComponent>()) {
			AssImpHelper::Locations locations;
			locations.model = this->model;
			locations.bones = this->bones;

			AssImpHelper::drawModel(_em, graphics, transform, skeleton, false, locations);
		}

		glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
		glCullFace(GL_BACK);
	}
}
