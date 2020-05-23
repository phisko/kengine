#pragma once

#include <glm/glm.hpp>
#include "Point.hpp"

namespace kengine {
	namespace matrixHelper {
		inline putils::Point3f getPos(const glm::mat4 & mat) { return { mat[3][0], mat[3][1], mat[3][2] }; }
	}
}