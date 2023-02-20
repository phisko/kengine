#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::glm {
	KENGINE_GLM_EXPORT putils::point3f extract_position(const ::glm::mat4 & mat) noexcept;
	KENGINE_GLM_EXPORT putils::vec3f extract_scale(const ::glm::mat4 & mat) noexcept;
	KENGINE_GLM_EXPORT putils::vec3f extract_rotation(const ::glm::mat4 & mat) noexcept;
}