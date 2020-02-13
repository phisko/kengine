#pragma once

#include <vector>
#include <string>
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

			putils_reflection_class_name(ModelSkeletonComponentMesh);
			putils_reflection_attributes(
				putils_reflection_attribute(&Mesh::boneNames)
			);
		};
		std::vector<Mesh> meshes;

		putils_reflection_class_name(ModelSkeletonComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ModelSkeletonComponent::meshes)
		);
		putils_reflection_used_types(
			putils_reflection_type(Mesh)
		);
	};
}
