#include "AssImpHelper.hpp"
#include "EntityManager.hpp"

#include "data/ModelComponent.hpp"
#include "data/TextureModelComponent.hpp"
#include "data/OpenGLModelComponent.hpp"
#include "systems/opengl/shaders/ShaderHelper.hpp"

#include "opengl/Program.hpp"

namespace kengine {
	namespace AssImpHelper {
		// declarations
		static void uploadDefaultBones(const SkeletonComponent & skeleton, const Uniforms & uniforms);
		static void bindTextures(EntityManager & em, unsigned int meshIndex, const AssImpTexturesModelComponent & textures, const Uniforms & uniforms);
		//
		void drawModel(EntityManager & em, const GraphicsComponent & graphics, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms) {
			if (graphics.model == Entity::INVALID_ID)
				return;

			const auto & modelInfoEntity = em.getEntity(graphics.model);
			if (!modelInfoEntity.has<ModelComponent>() || !modelInfoEntity.has<OpenGLModelComponent>() || !modelInfoEntity.has<AssImpTexturesModelComponent>())
				return;

			const auto & openGL = modelInfoEntity.get<OpenGLModelComponent>();
			const auto & modelInfo = modelInfoEntity.get<ModelComponent>();
			const auto & textures = modelInfoEntity.get<AssImpTexturesModelComponent>();

			uniforms.model = ShaderHelper::getModelMatrix(modelInfo, transform);

			const bool noSkeleton = skeleton.meshes.empty();
			if (noSkeleton)
				uploadDefaultBones(skeleton, uniforms);

			for (unsigned int i = 0; i < openGL.meshes.size(); ++i) {
				if (!noSkeleton)
					glUniformMatrix4fv(uniforms.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(skeleton.meshes[i].boneMatsBoneSpace[0]));

				if (useTextures)
					bindTextures(em, i, textures, uniforms);

				const auto & meshInfo = openGL.meshes[i];
				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, (GLsizei)meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}
		}

		static void uploadDefaultBones(const SkeletonComponent & skeleton, const Uniforms & uniforms) {
			static glm::mat4 defaultMats[KENGINE_SKELETON_MAX_BONES];
			static bool first = true;
			if (first) {
				for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i)
					defaultMats[i] = glm::mat4(1.f);
				first = false;
			}
			glUniformMatrix4fv(uniforms.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(defaultMats[0]));
		}

		// declaration
		static void bindTexture(EntityManager & em, size_t texture, Entity::ID modelID);
		//
		static void bindTextures(EntityManager & em, unsigned int meshIndex, const AssImpTexturesModelComponent & textures, const Uniforms & uniforms) {
			const auto & meshTextures = textures.meshes[meshIndex];

			{ // Diffuse
				if (!meshTextures.diffuse.empty())
					bindTexture(em, uniforms.diffuseTextureID, meshTextures.diffuse[0]);
				else
					uniforms.diffuseColor = meshTextures.diffuseColor;

				uniforms.hasTexture = !meshTextures.diffuse.empty();
			}

			{ // Specular
				const auto textureID = uniforms.specularTextureID;
				if (!meshTextures.specular.empty())
					bindTexture(em, textureID, meshTextures.specular[0]);
				else if (!meshTextures.diffuse.empty())
					bindTexture(em, textureID, meshTextures.diffuse[0]);
				else
					uniforms.specularColor = meshTextures.specularColor;
			}
		}

		static void bindTexture(EntityManager & em, size_t texture, Entity::ID modelID) {
			glActiveTexture((GLenum)(GL_TEXTURE0 + texture));
			const auto & modelEntity = em.getEntity(modelID);
			glBindTexture(GL_TEXTURE_2D, modelEntity.get<TextureModelComponent>().texture);
		}
	}
}