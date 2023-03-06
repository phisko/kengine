#include "set_bone_matrix.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

#include "get_bone_index.hpp"

namespace kengine::skeleton {
	static constexpr auto log_category = "skeleton";

	void set_bone_matrix(const entt::registry & r, const char * bone, const glm::mat4 & m, bone_matrices & matrices, const bone_names & names) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, log_category, "Setting bone matrix for {}", bone);

		const auto indexes = get_bone_index(r, bone, names);
		if (indexes.mesh_index >= matrices.meshes.size())
			return;

		auto & mesh = matrices.meshes[indexes.mesh_index];
		mesh.bone_mats_mesh_space[indexes.bone_index] = m;
	}
}