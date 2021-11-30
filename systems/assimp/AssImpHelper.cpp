#include "AssImpHelper.hpp"
#include "kengine.hpp"

#include "data/SystemSpecificTextureComponent.hpp"
#include "data/SystemSpecificModelComponent.hpp"
#include "helpers/matrixHelper.hpp"

#include "opengl/Program.hpp"
#include "opengl/Mesh.hpp"

namespace kengine::AssImpHelper {
	void drawModel(const InstanceComponent & instance, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms) noexcept {
		struct impl {
			static void drawModel(const InstanceComponent & instance, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms) noexcept {
				const auto model = entities[instance.model];

				const auto openGL = model.tryGet<SystemSpecificModelComponent<putils::gl::Mesh>>();
				if (!openGL)
					return;

				uniforms.model = matrixHelper::getModelMatrix(transform, model.tryGet<TransformComponent>());

				const bool noSkeleton = skeleton.meshes.empty();
				if (noSkeleton)
					uploadDefaultBones(skeleton, uniforms);

				const auto & textures = model.get<AssImpTexturesModelComponent>();

				for (unsigned int i = 0; i < openGL->meshes.size(); ++i) {
					if (!noSkeleton)
						glUniformMatrix4fv(uniforms.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(skeleton.meshes[i].boneMatsBoneSpace[0]));

					if (useTextures)
						bindTextures(i, textures, uniforms);

					putils::gl::draw(openGL->meshes[i]);
				}
			}

			static void uploadDefaultBones(const SkeletonComponent & skeleton, const Uniforms & uniforms) noexcept {
				static glm::mat4 defaultMats[KENGINE_SKELETON_MAX_BONES];
				static bool first = true;
				if (first) {
					for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i)
						defaultMats[i] = glm::mat4(1.f);
					first = false;
				}
				glUniformMatrix4fv(uniforms.bones, KENGINE_SKELETON_MAX_BONES, GL_FALSE, glm::value_ptr(defaultMats[0]));
			}

			static void bindTextures(unsigned int meshIndex, const AssImpTexturesModelComponent & textures, const Uniforms & uniforms) noexcept {
				const auto & meshTextures = textures.meshes[meshIndex];

				{ // Diffuse
					if (!meshTextures.diffuse.empty())
						bindTexture(uniforms.diffuseTextureID, meshTextures.diffuse[0]);
					else
						uniforms.diffuseColor = meshTextures.diffuseColor;

					uniforms.hasTexture = !meshTextures.diffuse.empty();
				}

				{ // Specular
					const auto textureID = uniforms.specularTextureID;
					if (!meshTextures.specular.empty())
						bindTexture(textureID, meshTextures.specular[0]);
					else if (!meshTextures.diffuse.empty())
						bindTexture(textureID, meshTextures.diffuse[0]);
					else
						uniforms.specularColor = meshTextures.specularColor;
				}
			}

			static void bindTexture(size_t texture, EntityID modelID) noexcept {
				glActiveTexture((GLenum)(GL_TEXTURE0 + texture));
				const auto modelEntity = entities[modelID];
				const auto openGL = modelEntity.tryGet<SystemSpecificTextureComponent<putils::gl::Texture>>();
				if (openGL)
					glBindTexture(GL_TEXTURE_2D, openGL->texture);
			}
		};

		impl::drawModel(instance, transform, skeleton, useTextures, uniforms);
	}
}