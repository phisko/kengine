#pragma once

#include <glm/glm.hpp>

namespace kengine {
	struct SkeletonComponent;
	struct ModelSkeletonComponent;

	namespace SkeletonHelper {
		struct BoneIndexes {
			unsigned int meshIndex = 0;
			unsigned int boneIndex = 0;
		};

		BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model);
		glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model);
		void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model);
	}
}