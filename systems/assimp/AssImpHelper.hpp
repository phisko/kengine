#pragma once

#ifndef KENGINE_ASSIMP_BONE_INFO_PER_VERTEX
# define KENGINE_ASSIMP_BONE_INFO_PER_VERTEX 4
#endif

#ifndef KENGINE_ASSIMP_BONE_MAX_PARENTS
# define KENGINE_ASSIMP_BONE_MAX_PARENTS 64
#endif

#ifndef KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH
# define KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH 256
#endif

#include "opengl/Uniform.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/SkeletonComponent.hpp"

namespace kengine {
	class EntityManager;

	struct AssImpObjectComponent {};

	struct AssImpTexturesModelComponent {
		struct MeshTextures {
			std::vector<Entity::ID> diffuse; // id of entities with TextureModelComponent
			std::vector<Entity::ID> specular;

			putils::NormalizedColor diffuseColor;
			putils::NormalizedColor specularColor;
		};

		std::vector<MeshTextures> meshes; // Must match those in ModelInfoComponent
	};

	namespace AssImpHelper {
		struct Uniforms {
			putils::gl::Uniform<glm::mat4> model;
			GLint bones;

			putils::gl::Uniform<bool> hasTexture;
			size_t diffuseTextureID;
			size_t specularTextureID;

			putils::gl::Uniform<putils::NormalizedColor> diffuseColor;
			putils::gl::Uniform<putils::NormalizedColor> specularColor;
		};

		void drawModel(EntityManager & em, const GraphicsComponent & model, const TransformComponent & transform, const SkeletonComponent & skeleton, bool useTextures, const Uniforms & uniforms);
	}
}
