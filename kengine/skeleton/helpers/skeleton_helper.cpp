#ifdef KENGINE_GLM

#include "skeleton_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::skeleton_helper {
	bone_indices get_bone_index(const entt::registry & r, const char * bone, const data::model_skeleton & model) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "skeleton_helper", "Getting bone index for %s", bone);

		bone_indices indexes;

		indexes.mesh_index = 0;
		for (const auto & mesh : model.meshes) {
			indexes.bone_index = 0;
			for (const auto & name : mesh.bone_names) {
				if (name == bone)
					return indexes;
				++indexes.bone_index;
			}
			++indexes.mesh_index;
		}

		kengine_assert_failed(r, "'", bone, "' bone not found");
		return indexes;
	}

	glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const data::skeleton & skeleton, const data::model_skeleton & model) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "skeleton_helper", "Getting bone matrix for %s", bone);

		const auto indexes = get_bone_index(r, bone, model);
		if (indexes.mesh_index >= skeleton.meshes.size())
			return glm::mat4(1.f);

		const auto & mesh = skeleton.meshes[indexes.mesh_index];
		return mesh.bone_mats_mesh_space[indexes.bone_index];
	}

	void set_bone_matrix(const entt::registry & r, const char * bone, const glm::mat4 & m, data::skeleton & skeleton, const data::model_skeleton & model) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "skeleton_helper", "Setting bone matrix for %s", bone);

		const auto indexes = get_bone_index(r, bone, model);
		if (indexes.mesh_index >= skeleton.meshes.size())
			return;

		auto & mesh = skeleton.meshes[indexes.mesh_index];
		mesh.bone_mats_mesh_space[indexes.bone_index] = m;
	}
}

#endif