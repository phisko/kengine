#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::data {
	struct transform;
}

namespace kengine::glm {
	KENGINE_GLM_EXPORT ::glm::mat4 get_model_matrix(const data::transform & transform, const data::transform * model_transform = nullptr) noexcept;
}
