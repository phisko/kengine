#include "AssImpShadowCube.hpp"

#include "EntityManager.hpp"

#include "components/TexturedModelComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "helpers/ShaderHelper.hpp"
#include "helpers/LightHelper.hpp"

#include "systems/opengl/shaders/shaders.hpp"
#include "TexturedShaderSrc.hpp"

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
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

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

		for (const auto &[e, textured, modelComp, transform, skeleton] : _em.getEntities<TexturedModelComponent, kengine::ModelComponent, kengine::TransformComponent3f, SkeletonComponent>()) {
			const auto & modelInfoEntity = _em.getEntity(modelComp.modelInfo);
			if (!modelInfoEntity.has<ModelInfoComponent>())
				continue;
			const auto & modelInfo = modelInfoEntity.get<ModelInfoComponent>();

			putils::gl::setUniform(this->model, ShaderHelper::getModelMatrix(modelInfo, transform));

			if (skeleton.meshes.empty()) {
				static putils::vector<glm::mat4, KENGINE_SKELETON_MAX_BONES> defaultMats;
				static bool first = true;
				if (first) {
					for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i)
						defaultMats.push_back(glm::mat4(1.f));
					first = false;
				}
				glUniformMatrix4fv(bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(*defaultMats.begin()));
			}

			for (unsigned int i = 0; i < modelInfo.meshes.size(); ++i) {
				const auto & meshInfo = modelInfo.meshes[i];

				if (!skeleton.meshes.empty())
					glUniformMatrix4fv(bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(*skeleton.meshes[i].boneMats.begin()));

				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}

		}

		glViewport(0, 0, screenWidth, screenHeight);
		glCullFace(GL_BACK);
	}
}
