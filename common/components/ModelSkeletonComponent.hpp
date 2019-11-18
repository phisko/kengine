#pragma once

#include "vector.hpp"
#include "string.hpp"

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
	struct ModelSkeletonComponent {
		struct Mesh {
			using string = putils::string<KENGINE_BONE_NAME_MAX_LENGTH>;
			putils::vector<string, KENGINE_SKELETON_MAX_BONES> boneNames;

			putils_reflection_class_name(ModelSkeletonComponentMesh);
			putils_reflection_attributes(
				putils_reflection_attribute(&Mesh::boneNames)
			);
		};
		putils::vector<Mesh, KENGINE_MAX_MESHES_PER_MODEL> meshes;

		putils_reflection_class_name(ModelSkeletonComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ModelSkeletonComponent::meshes)
		);
	};
}
