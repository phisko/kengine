#pragma once

#include <glm/glm.hpp>
#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	static inline glm::ivec2 toglm(const putils::Point2i & p) noexcept {
		return { p.x, p.y };
	}

	static inline glm::vec2 toglm(const putils::Point2f & p) noexcept {
		return { p.x, p.y };
	}

	static inline glm::vec3 toglm(const putils::Point3f & p) noexcept {
		return { p.x, p.y, p.z };
	}

	static inline glm::vec4 toglm(const putils::NormalizedColor & c) noexcept {
		return { c.r, c.g, c.b, c.a };
	}
}