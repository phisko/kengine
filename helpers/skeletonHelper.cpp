#ifdef KENGINE_GLM

#include "skeletonHelper.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::skeletonHelper {
	BoneIndexes getBoneIndex(const entt::registry & r, const char * bone, const ModelSkeletonComponent & model) noexcept {
		KENGINE_PROFILING_SCOPE;

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

		kengine_assert_failed(r, "'", bone, "' bone not found");
		return indexes;
	}

	glm::mat4 getBoneMatrix(const entt::registry & r, const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto indexes = getBoneIndex(r, bone, model);
		if (indexes.meshIndex >= skeleton.meshes.size())
			return glm::mat4(1.f);

		const auto & mesh = skeleton.meshes[indexes.meshIndex];
		return mesh.boneMatsMeshSpace[indexes.boneIndex];
	}

	void setBoneMatrix(const entt::registry & r, const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto indexes = getBoneIndex(r, bone, model);
		if (indexes.meshIndex >= skeleton.meshes.size())
			return;

		auto & mesh = skeleton.meshes[indexes.meshIndex];
		mesh.boneMatsMeshSpace[indexes.boneIndex] = m;
	}
}

#endif