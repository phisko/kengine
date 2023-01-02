#pragma once

// stl
#include <vector>

// glm
#include <glm/glm.hpp>

// reflection
#include "putils/reflection.hpp"

#ifndef KENGINE_SKELETON_MAX_BONES
#define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine::data {
	struct skeleton {
		struct mesh {
			glm::mat4 bone_mats_bone_space[KENGINE_SKELETON_MAX_BONES]; // Used by shader
			glm::mat4 bone_mats_mesh_space[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space
		};
		std::vector<mesh> meshes;
	};
}

#define refltype kengine::data::skeleton
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(meshes)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::mesh)
	);
};
#undef refltype

#define refltype kengine::data::skeleton::mesh
putils_reflection_info {
	putils_reflection_custom_class_name(SkeletonComponentMesh);
	putils_reflection_attributes(
		putils_reflection_attribute(bone_mats_bone_space),
		putils_reflection_attribute(bone_mats_mesh_space)
	);
};
#undef refltype
