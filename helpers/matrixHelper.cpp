#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrixHelper.hpp"

#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"

namespace kengine::matrixHelper {
	glm::mat4 getModelMatrix(const ModelComponent & modelInfo, const TransformComponent & transform) {
		glm::mat4 model(1.f);
		const auto & centre = transform.boundingBox.position;

		{ // TransformComponent
			model = glm::translate(model, toVec(centre));

			model = glm::rotate(model,
				transform.yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::rotate(model,
				transform.pitch,
				{ 1.f, 0.f, 0.f }
			);
			model = glm::rotate(model,
				transform.roll,
				{ 0.f, 0.f, 1.f }
			);

			model = glm::scale(model, toVec(transform.boundingBox.size));
		}

		{ // GraphicsComponent
			model = glm::rotate(model,
				modelInfo.yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::rotate(model,
				modelInfo.pitch,
				{ 1.f, 0.f, 0.f }
			);
			model = glm::rotate(model,
				modelInfo.roll,
				{ 0.f, 0.f, 1.f }
			);

			model = glm::translate(model, -toVec(modelInfo.boundingBox.position)); // Re-center
			model = glm::scale(model, toVec(modelInfo.boundingBox.size));
		}

		return model;
	}
}
