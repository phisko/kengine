#pragma once

// glm
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::data {
	struct transform;
}

namespace kengine::matrix_helper {
	KENGINE_GLM_EXPORT glm::vec3 to_vec(const putils::point3f & pos) noexcept;

	KENGINE_GLM_EXPORT putils::point3f get_position(const glm::mat4 & mat) noexcept;
	KENGINE_GLM_EXPORT putils::vec3f get_scale(const glm::mat4 & mat) noexcept;
	KENGINE_GLM_EXPORT putils::vec3f get_rotation(const glm::mat4 & mat) noexcept;

	KENGINE_GLM_EXPORT putils::point3f convert_to_referencial(const putils::point3f & pos, const glm::mat4 & conversion_matrix) noexcept;

	KENGINE_GLM_EXPORT glm::mat4 get_model_matrix(const data::transform & transform, const data::transform * model_transform = nullptr) noexcept;
}