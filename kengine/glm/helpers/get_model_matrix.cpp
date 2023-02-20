#include "get_model_matrix.hpp"

// glm
#include <glm/gtc/matrix_transform.hpp>

// kengine core
#include "kengine/core/data/transform.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine glm
#include "kengine/glm/helpers/to_vec.hpp"

namespace kengine::glm {
	::glm::mat4 get_model_matrix(const data::transform & transform, const data::transform * model_transform) noexcept {
		KENGINE_PROFILING_SCOPE;

		::glm::mat4 model(1.f);

		{ // object
			model = ::glm::translate(model, kengine::glm::to_vec(transform.bounding_box.position));

			model = ::glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
			model = ::glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
			model = ::glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });

			model = ::glm::scale(model, kengine::glm::to_vec(transform.bounding_box.size));
		}

		if (model_transform != nullptr) { // Model
			model = ::glm::scale(model, kengine::glm::to_vec(model_transform->bounding_box.size));

			model = ::glm::rotate(model, model_transform->yaw, { 0.f, 1.f, 0.f });
			model = ::glm::rotate(model, model_transform->pitch, { 1.f, 0.f, 0.f });
			model = ::glm::rotate(model, model_transform->roll, { 0.f, 0.f, 1.f });

			model = ::glm::translate(model, kengine::glm::to_vec(model_transform->bounding_box.position)); // Re-center
		}

		return model;
	}
}