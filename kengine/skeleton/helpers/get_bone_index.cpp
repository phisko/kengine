#include "get_bone_index.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::skeleton {
	bone_indices get_bone_index(const entt::registry & r, const char * bone, const model_skeleton & model) noexcept {
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
}