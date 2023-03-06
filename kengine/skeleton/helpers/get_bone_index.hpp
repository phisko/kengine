#pragma once

// entt
#include <entt/entity/fwd.hpp>

// glm
#include <glm/glm.hpp>

// kengine
#include "kengine/skeleton/data/bone_matrices.hpp"
#include "kengine/skeleton/data/bone_names.hpp"

namespace kengine::skeleton {
	struct bone_indices {
		unsigned int mesh_index = 0;
		unsigned int bone_index = 0;
	};

	KENGINE_SKELETON_EXPORT bone_indices get_bone_index(const entt::registry & r, const char * bone, const bone_names & names) noexcept;
}
