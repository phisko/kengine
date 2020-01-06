#pragma once

#include <glm/glm.hpp>
#include <vector>

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
