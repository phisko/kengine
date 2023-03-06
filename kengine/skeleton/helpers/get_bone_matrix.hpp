#pragma once

// entt
#include <entt/entity/fwd.hpp>

// glm
#include <glm/glm.hpp>

// kengine
#include "kengine/skeleton/data/skeleton.hpp"
#include "kengine/skeleton/data/bone_names.hpp"

namespace kengine::skeleton {
	KENGINE_SKELETON_EXPORT ::glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const skeleton & skeleton, const bone_names & model) noexcept;
}