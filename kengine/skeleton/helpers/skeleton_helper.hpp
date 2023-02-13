#pragma once

// entt
#include <entt/entity/fwd.hpp>

// glm
#include <glm/glm.hpp>

// kengine skeleton
#include "kengine/skeleton/data/skeleton.hpp"
#include "kengine/skeleton/data/model_skeleton.hpp"

namespace kengine::skeleton_helper {
	struct bone_indices {
		unsigned int mesh_index = 0;
		unsigned int bone_index = 0;
	};

	KENGINE_SKELETON_EXPORT bone_indices get_bone_index(const entt::registry & r, const char * bone, const data::model_skeleton & model) noexcept;
	KENGINE_SKELETON_EXPORT glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const data::skeleton & skeleton, const data::model_skeleton & model) noexcept;
	KENGINE_SKELETON_EXPORT void set_bone_matrix(const entt::registry & r, const char * bone, const glm::mat4 & m, data::skeleton & skeleton, const data::model_skeleton & model) noexcept;
}
