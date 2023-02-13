#include "matrix_helper.hpp"

// glm
#include <glm/gtc/matrix_transform.hpp>

// kengine core
#include "kengine/core/data/transform.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

namespace kengine::matrix_helper {
	glm::vec3 to_vec(const putils::point3f & pos) noexcept {
		return { pos.x, pos.y, pos.z };
	}

	putils::point3f get_position(const glm::mat4 & mat) noexcept {
		return { mat[3][0], mat[3][1], mat[3][2] };
	}

	putils::vec3f get_scale(const glm::mat4 & mat) noexcept {
		return {
			putils::get_length(putils::vec3f{ mat[0][0], mat[0][1], mat[0][2] }),
			putils::get_length(putils::vec3f{ mat[1][0], mat[1][1], mat[1][2] }),
			putils::get_length(putils::vec3f{ mat[2][0], mat[2][1], mat[2][2] })
		};
	}

	putils::vec3f get_rotation(const glm::mat4 & mat) noexcept {
		putils::point3f ret;
		glm::extractEulerAngleYXZ(mat, ret.y, ret.x, ret.z);
		return ret;
	}

	putils::point3f convert_to_referencial(const putils::point3f & pos, const glm::mat4 & conversion_matrix) noexcept {
		const auto tmp = conversion_matrix * glm::vec4(to_vec(pos), 1.f);
		return { tmp.x, tmp.y, tmp.z };
	}

	glm::mat4 get_model_matrix(const data::transform & transform, const data::transform * model_transform) noexcept {
		KENGINE_PROFILING_SCOPE;

		glm::mat4 model(1.f);

		{ // object
			model = glm::translate(model, to_vec(transform.bounding_box.position));

			model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
			model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
			model = glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });

			model = glm::scale(model, to_vec(transform.bounding_box.size));
		}

		if (model_transform != nullptr) { // Model
			model = glm::scale(model, to_vec(model_transform->bounding_box.size));

			model = glm::rotate(model, model_transform->yaw, { 0.f, 1.f, 0.f });
			model = glm::rotate(model, model_transform->pitch, { 1.f, 0.f, 0.f });
			model = glm::rotate(model, model_transform->roll, { 0.f, 0.f, 1.f });

			model = glm::translate(model, to_vec(model_transform->bounding_box.position)); // Re-center
		}

		return model;
	}
}