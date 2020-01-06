#pragma once

#include <glm/glm.hpp>
#include "data/SkeletonComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"

namespace kengine {
	namespace SkeletonHelper {
		struct BoneIndexes {
			unsigned int meshIndex = 0;
			unsigned int boneIndex = 0;
		};

		static BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model);
		static glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model);
		static void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model);

		static BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model) {
			BoneIndexes indexes;

			indexes.meshIndex = 0;
			for (const auto & mesh : model.meshes) {
				indexes.boneIndex = 0;
				for (const auto & name : mesh.boneNames) {
					if (name == bone)
						return indexes;
					++indexes.boneIndex;
				}
				++indexes.meshIndex;
			}

			assert(false); // Not found
			return indexes;
		}

		static glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model) {
			const auto indexes = getBoneIndex(bone, model);
			if (indexes.meshIndex >= skeleton.meshes.size())
				return glm::mat4(1.f);

			const auto & mesh = skeleton.meshes[indexes.meshIndex];
			return mesh.boneMatsMeshSpace[indexes.boneIndex];
		}

		static void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model) {
			const auto indexes = getBoneIndex(bone, model);
			if (indexes.meshIndex >= skeleton.meshes.size())
				return;

			auto & mesh = skeleton.meshes[indexes.meshIndex];
			mesh.boneMatsMeshSpace[indexes.boneIndex] = m;
		}
	}
}