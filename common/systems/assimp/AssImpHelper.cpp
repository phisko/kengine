#include "AssImpHelper.hpp"
#include "EntityManager.hpp"

#include "components/ModelComponent.hpp"
#include "components/TextureModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "common/systems/opengl/ShaderHelper.hpp"

#include "opengl/Program.hpp"

namespace kengine {
	namespace AssImpHelper {
		void drawModel(EntityManager & em, const GraphicsComponent & graphics, const TransformComponent3f & transform, const SkeletonComponent & skeleton, bool useTextures, const Locations & locations) {
			if (graphics.model == Entity::INVALID_ID)
				return;

			const auto & modelInfoEntity = em.getEntity(graphics.model);
			if (!modelInfoEntity.has<ModelComponent>() || !modelInfoEntity.has<OpenGLModelComponent>() || !modelInfoEntity.has<AssImpTexturesModelComponent>())
				return;

			const auto & openGL = modelInfoEntity.get<OpenGLModelComponent>();
			const auto & modelInfo = modelInfoEntity.get<ModelComponent>();
			const auto & textures = modelInfoEntity.get<AssImpTexturesModelComponent>();

			putils::gl::setUniform(locations.model, ShaderHelper::getModelMatrix(modelInfo, transform));

			if (skeleton.meshes.empty()) {
				static glm::mat4 defaultMats[KENGINE_SKELETON_MAX_BONES];
				static bool first = true;
				if (first) {
					for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i)
						defaultMats[i] = glm::mat4(1.f);
					first = false;
				}
				glUniformMatrix4fv(locations.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(defaultMats[0]));
			}

			for (unsigned int i = 0; i < openGL.meshes.size(); ++i) {
				if (!skeleton.meshes.empty())
					glUniformMatrix4fv(locations.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(skeleton.meshes[i].boneMatsBoneSpace[0]));

				if (useTextures) {
					const auto & meshTextures = textures.meshes[i];
					if (!meshTextures.diffuse.empty()) {
						glActiveTexture((GLenum)(GL_TEXTURE0 + locations.diffuseTextureID));
						const auto & modelEntity = em.getEntity(meshTextures.diffuse[0]);
						glBindTexture(GL_TEXTURE_2D, modelEntity.get<TextureModelComponent>().texture);
					}
					else
						putils::gl::setUniform(locations.diffuseColor, meshTextures.diffuseColor);

					putils::gl::setUniform(locations.hasTexture, meshTextures.diffuse.empty() ? 0 : 1);

					// glActiveTexture(GL_TEXTURE0 + locations.specularTextureID);
					// if (!meshTextures.specular.empty())
					// 	glBindTexture(GL_TEXTURE_2D, meshTextures.specular[0]);
					// else if (!meshTextures.diffuse.empty())
					// 	glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);
					// else
					// 	putils::gl::setUniform(locations.specularColor, meshTextures.specularColor);
				}

				const auto & meshInfo = openGL.meshes[i];
				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, (GLsizei)meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}
		}
	}
}