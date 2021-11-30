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
		};
		std::vector<Mesh> meshes;
	};
}

#define refltype kengine::SkeletonComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(meshes)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::Mesh)
	);
};
#undef refltype

#define refltype kengine::SkeletonComponent::Mesh
putils_reflection_info {
	putils_reflection_custom_class_name(SkeletonComponentMesh);
	putils_reflection_attributes(
		putils_reflection_attribute(boneMatsBoneSpace),
		putils_reflection_attribute(boneMatsMeshSpace)
	);
};
#undef refltype
