#pragma once

#ifdef KENGINE_GLM

// glm
#include <glm/glm.hpp>

// kengine data
#include "data/SkeletonComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"

namespace kengine::skeletonHelper {
	struct BoneIndexes {
		unsigned int meshIndex = 0;
		unsigned int boneIndex = 0;
	};

	BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model) noexcept;
	glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept;
	void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept;
}

#endif