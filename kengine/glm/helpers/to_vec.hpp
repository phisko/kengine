#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::glm {
	inline ::glm::vec3 to_vec(const putils::point3f & pos) noexcept {
		return { pos.x, pos.y, pos.z };
	}
}