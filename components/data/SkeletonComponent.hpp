#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "reflection.hpp"

#ifndef KENGINE_SKELETON_MAX_BONES
# define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine {
	struct SkeletonComponent {
		struct Mesh {
			glm::mat4 boneMatsBoneSpace[KENGINE_SKELETON_MAX_BONES]; // Used by shader
			glm::mat4 boneMatsMeshSpace[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space

			putils_reflection_class_name(SkeletonComponentMesh);
			putils_reflection_attributes(
				putils_reflection_attribute(&Mesh::boneMatsBoneSpace),
				putils_reflection_attribute(&Mesh::boneMatsMeshSpace)
			);
		};
		std::vector<Mesh> meshes;
		putils_reflection_class_name(SkeletonComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&SkeletonComponent::meshes)
		);
		putils_reflection_used_types(
			putils_reflection_type(Mesh)
		);
	};
}
