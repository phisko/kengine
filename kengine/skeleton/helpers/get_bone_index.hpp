#pragma once

// entt
#include <entt/entity/fwd.hpp>

// glm
#include <glm/glm.hpp>

// kengine skeleton
#include "kengine/skeleton/data/skeleton.hpp"
#include "kengine/skeleton/data/model_skeleton.hpp"

namespace kengine::skeleton {
	struct bone_indices {
		unsigned int mesh_index = 0;
		unsigned int bone_index = 0;
	};

	KENGINE_SKELETON_EXPORT bone_indices get_bone_index(const entt::registry & r, const char * bone, const model_skeleton & model) noexcept;
}