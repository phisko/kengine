#include "AssImpShadowMap.hpp"
#include "EntityManager.hpp"

#include "components/SkeletonComponent.hpp"
#include "components/TexturedModelComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "helpers/ShaderHelper.hpp"

#include "TexturedShaderSrc.hpp"

namespace kengine {
	void AssImpShadowMap::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<AssImpShadowMap>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
	}

	template<typename T>
	void AssImpShadowMap::runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		if (!e.has<DepthMapComponent>())
			return;

		glViewport(0, 0, KENGINE_SHADOW_MAP_SIZE, KENGINE_SHADOW_MAP_SIZE);

		const auto & depthMap = e.get<DepthMapComponent>();
		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

		use();
		putils::gl::setUniform(view, LightHelper::getLightSpaceMatrix(light, { pos.x, pos.y, pos.z }, screenWidth, screenHeight));
		glCullFace(GL_FRONT);

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

		glCullFace(GL_BACK);
		glViewport(0, 0, screenWidth, screenHeight);
	}

	void AssImpShadowMap::run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
	void AssImpShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
}
