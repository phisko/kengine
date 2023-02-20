#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"
#include "putils/color.hpp"

namespace kengine {
	static inline ::glm::ivec2 toglm(const putils::point2i & p) noexcept {
		return { p.x, p.y };
	}

	static inline ::glm::vec2 toglm(const putils::point2f & p) noexcept {
		return { p.x, p.y };
	}

	static inline ::glm::vec3 toglm(const putils::point3f & p) noexcept {
		return { p.x, p.y, p.z };
	}

	static inline ::glm::vec4 toglm(const putils::normalized_color & c) noexcept {
		return { c.r, c.g, c.b, c.a };
	}
}