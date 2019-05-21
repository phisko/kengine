#include "AssImpHelper.hpp"

#include "components/ModelInfoComponent.hpp"
#include "helpers/ShaderHelper.hpp"

#include "opengl/Program.hpp"

namespace kengine {
	namespace AssImpHelper {
		void drawModel(kengine::EntityManager & em, const kengine::ModelComponent & model, const kengine::TransformComponent3f & transform, const kengine::SkeletonComponent & skeleton, bool useTextures, const Locations & locations) {
			const auto & modelInfoEntity = em.getEntity(model.modelInfo);
			if (!modelInfoEntity.has<kengine::ModelInfoComponent>())
				return;
			const auto & modelInfo = modelInfoEntity.get<kengine::ModelInfoComponent>();
			const auto & textures = modelInfoEntity.get<kengine::ModelInfoTexturesComponent>();

			putils::gl::setUniform(locations.model, ShaderHelper::getModelMatrix(modelInfo, transform));

			if (skeleton.meshes.empty()) {
				static putils::vector<glm::mat4, KENGINE_SKELETON_MAX_BONES> defaultMats;
				static bool first = true;
				if (first) {
					for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i)
						defaultMats.push_back(glm::mat4(1.f));
					first = false;
				}
				glUniformMatrix4fv(locations.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(*defaultMats.begin()));
			}

			for (unsigned int i = 0; i < modelInfo.meshes.size(); ++i) {
				if (!skeleton.meshes.empty())
					glUniformMatrix4fv(locations.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(*skeleton.meshes[i].boneMats.begin()));

				if (useTextures) {
					const auto & meshTextures = textures.meshes[i];
					if (!meshTextures.diffuse.empty()) {
						glActiveTexture(GL_TEXTURE0 + locations.diffuseTextureID);
						glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);
					}
					else
						putils::gl::setUniform(locations.diffuseColor, meshTextures.diffuseColor);

					putils::gl::setUniform(locations.hasTexture, meshTextures.diffuse.empty() ? 0 : 1);

					glActiveTexture(GL_TEXTURE0 + locations.specularTextureID);
					if (!meshTextures.specular.empty())
						glBindTexture(GL_TEXTURE_2D, meshTextures.specular[0]);
					else if (!meshTextures.diffuse.empty())
						glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);
					else
						putils::gl::setUniform(locations.specularColor, meshTextures.specularColor);
				}

				const auto & meshInfo = modelInfo.meshes[i];
				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}
		}
	}
}