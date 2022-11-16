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

	KENGINE_CORE_EXPORT BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model) noexcept;
	KENGINE_CORE_EXPORT glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept;
	KENGINE_CORE_EXPORT void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept;
}

#endif