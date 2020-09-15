#include "AssImpHelper.hpp"
#include "EntityManager.hpp"

#include "data/ModelComponent.hpp"
#include "data/SystemSpecificTextureComponent.hpp"
#include "data/SystemSpecificModelComponent.hpp"
#include "helpers/matrixHelper.hpp"

#include "opengl/Program.hpp"
#include "opengl/Mesh.hpp"

namespace kengine {
	namespace AssImpHelper {
#pragma region drawModel
#pragma region declarations
		static void uploadDefaultBones(const SkeletonComponent & skeleton, const Uniforms & uniforms);
		static void bindTextures(EntityManager & em, unsigned int meshIndex, const AssImpTexturesModelComponent & textures, const Uniforms & uniforms);
#pragma endregion
		void drawModel(EntityManager & em, const InstanceComponent & instance, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms) {
			const auto model = em.getEntity(instance.model);

			const auto openGL = model.tryGet<SystemSpecificModelComponent<putils::gl::Mesh>>();
			if (!openGL)
				return;

			uniforms.model = matrixHelper::getModelMatrix(model.get<ModelComponent>(), transform);

			const bool noSkeleton = skeleton.meshes.empty();
			if (noSkeleton)
				uploadDefaultBones(skeleton, uniforms);

			const auto & textures = model.get<AssImpTexturesModelComponent>();

			for (unsigned int i = 0; i < openGL->meshes.size(); ++i) {
				if (!noSkeleton)
					glUniformMatrix4fv(uniforms.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(skeleton.meshes[i].boneMatsBoneSpace[0]));

				if (useTextures)
					bindTextures(em, i, textures, uniforms);

				putils::gl::draw(openGL->meshes[i]);
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

#pragma region bindTextures
#pragma region declarations
		static void bindTexture(EntityManager & em, size_t texture, Entity::ID modelID);
#pragma endregion
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
			const auto modelEntity = em.getEntity(modelID);
			const auto openGL = modelEntity.tryGet<SystemSpecificTextureComponent<putils::gl::Texture>>();
			if (openGL)
				glBindTexture(GL_TEXTURE_2D, openGL->texture);
		}
#pragma endregion bindTextures

#pragma endregion drawModel
	}
}