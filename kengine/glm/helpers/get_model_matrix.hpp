#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

// kengine
#include "kengine/core/data/transform.hpp"

namespace kengine::glm {
	KENGINE_GLM_EXPORT ::glm::mat4 get_model_matrix(const core::transform & transform, const core::transform * model_transform = nullptr) noexcept;
}
