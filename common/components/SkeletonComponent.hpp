#pragma once

#include <glm/glm.hpp>
#include "vector.hpp"
#include "string.hpp"
#include "not_serializable.hpp"

#ifndef KENGINE_BONE_NAME_MAX_LENGTH
# define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_MAX_MESHES_PER_MODEL
# define KENGINE_MAX_MESHES_PER_MODEL 64
#endif

#ifndef KENGINE_SKELETON_MAX_BONES
# define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine {
	struct ModelSkeletonComponent : kengine::not_serializable {
		struct Mesh {
			using string = putils::string<KENGINE_BONE_NAME_MAX_LENGTH>;
			putils::vector<string, KENGINE_SKELETON_MAX_BONES> boneNames;

			pmeta_get_class_name(ModelSkeletonComponentMesh);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Mesh::boneNames)
			);
		};
		putils::vector<Mesh, KENGINE_MAX_MESHES_PER_MODEL> meshes;

		pmeta_get_class_name(ModelSkeletonComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ModelSkeletonComponent::meshes)
		);
	};

	struct SkeletonComponent : kengine::not_serializable {
		struct Mesh {
			glm::mat4 boneMatsBoneSpace[KENGINE_SKELETON_MAX_BONES]; // Used by shader
			glm::mat4 boneMatsMeshSpace[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space
		};
		putils::vector<Mesh, KENGINE_MAX_MESHES_PER_MODEL> meshes;

		pmeta_get_class_name(SkeletonComponent);
	};
}
