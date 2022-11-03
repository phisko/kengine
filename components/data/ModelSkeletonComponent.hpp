#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "reflection.hpp"

#ifndef KENGINE_BONE_NAME_MAX_LENGTH
# define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_SKELETON_MAX_BONES
# define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine {
	struct ModelSkeletonComponent {
		struct Mesh {
			std::vector<std::string> boneNames;
		};
		std::vector<Mesh> meshes;
	};
}

#define refltype kengine::ModelSkeletonComponent
putils_reflection_info {
	putils_reflection_custom_class_name(ModelSkeletonComponent);
	putils_reflection_attributes(
		putils_reflection_attribute(meshes)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::Mesh)
	);
};
#undef refltype

#define refltype kengine::ModelSkeletonComponent::Mesh
putils_reflection_info {
	putils_reflection_custom_class_name(ModelSkeletonComponentMesh);
	putils_reflection_attributes(
		putils_reflection_attribute(boneNames)
	);
};
#undef refltype
