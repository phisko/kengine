#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::glm {
	KENGINE_GLM_EXPORT putils::point3f convert_to_referencial(const putils::point3f & pos, const ::glm::mat4 & conversion_matrix) noexcept;
}