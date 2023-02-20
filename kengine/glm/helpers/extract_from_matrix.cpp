#include "extract_from_matrix.hpp"

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace kengine::glm {
	putils::point3f extract_position(const ::glm::mat4 & mat) noexcept {
		return { mat[3][0], mat[3][1], mat[3][2] };
	}

	putils::vec3f extract_scale(const ::glm::mat4 & mat) noexcept {
		return {
			putils::get_length(putils::vec3f{ mat[0][0], mat[0][1], mat[0][2] }),
			putils::get_length(putils::vec3f{ mat[1][0], mat[1][1], mat[1][2] }),
			putils::get_length(putils::vec3f{ mat[2][0], mat[2][1], mat[2][2] })
		};
	}

	putils::vec3f extract_rotation(const ::glm::mat4 & mat) noexcept {
		putils::point3f ret;
		::glm::extractEulerAngleYXZ(mat, ret.y, ret.x, ret.z);
		return ret;
	}
}