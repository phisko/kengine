#include "get_bone_index.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::skeleton {
	static constexpr auto log_category = "skeleton";

	bone_indices get_bone_index(const entt::registry & r, const char * bone, const bone_names & names) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, log_category, "Getting bone index for {}", bone);

		bone_indices indexes;

		indexes.mesh_index = 0;
		for (const auto & mesh : names.meshes) {
			indexes.bone_index = 0;
			for (const auto & name : mesh.bone_names) {
				if (name == bone)
					return indexes;
				++indexes.bone_index;
			}
			++indexes.mesh_index;
		}

		kengine_assert_failed(r, "'{}' bone not found", bone);
		return indexes;
	}
}