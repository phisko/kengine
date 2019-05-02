#include "TexturedShader.hpp"

#include "components/TransformComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/TexturedModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "helpers/ShaderHelper.hpp"

#include "AssImpDefines.h"

#include "TexturedShaderSrc.hpp"
static_assert(KENGINE_ASSIMP_BONE_INFO_PER_VERTEX == 4, "This shader assumes only 4 bones per vertex");

namespace kengine {
	void TexturedShader::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<TexturedShader>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::TexturedShader::frag, GL_FRAGMENT_SHADER }
		));

		_diffuseTextureID = firstTextureID;
		putils::gl::setUniform(texture_diffuse, _diffuseTextureID);

		_specularTextureID = _diffuseTextureID + 1;
		putils::gl::setUniform(texture_specular, _specularTextureID);
	}

		void TexturedShader::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
			use();

			glEnable(GL_DEPTH_TEST);

			putils::gl::setUniform(this->view, view);
			putils::gl::setUniform(this->proj, proj);

			for (const auto &[e, textured, modelComp, transform, skeleton] : _em.getEntities<TexturedModelComponent, kengine::ModelComponent, kengine::TransformComponent3f, SkeletonComponent>()) {
				const auto & modelInfoEntity = _em.getEntity(modelComp.modelInfo);
				if (!modelInfoEntity.has<kengine::ModelInfoComponent>())
					continue;
				const auto & modelInfo = modelInfoEntity.get<kengine::ModelInfoComponent>();
				const auto & textures = modelInfoEntity.get<kengine::ModelInfoTexturesComponent>();

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
					const auto & meshTextures = textures.meshes[i];

					if (!skeleton.meshes.empty())
						glUniformMatrix4fv(bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(*skeleton.meshes[i].boneMats.begin()));

					assert(!meshTextures.diffuse.empty());
					glActiveTexture(GL_TEXTURE0 + _diffuseTextureID);
					glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);

					glActiveTexture(GL_TEXTURE0 + _specularTextureID);
					if (meshTextures.specular.empty())
						glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);
					else
						glBindTexture(GL_TEXTURE_2D, meshTextures.specular[0]);

					glBindVertexArray(meshInfo.vertexArrayObject);
					glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
					glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
				}
			}

			glDisable(GL_DEPTH_TEST);
		}
}