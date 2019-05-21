#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "vector.hpp"

#ifndef KENGINE_SKELETON_MAX_BONES
# define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine {
	struct SkeletonComponent : kengine::not_serializable {
		struct Mesh {
			putils::vector<glm::mat4, KENGINE_SKELETON_MAX_BONES> boneMats;
		};
		std::vector<Mesh> meshes;

		pmeta_get_class_name(SkeletonComponent);
	};
}
