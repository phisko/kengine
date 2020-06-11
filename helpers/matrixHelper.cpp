#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrixHelper.hpp"

#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"

#include "helpers/cameraHelper.hpp"

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

	glm::mat4 getProjMatrix(const CameraComponent & cam, const ViewportComponent & viewport, float nearPlane, float farPlane) {
		return glm::perspective(
			cam.frustum.size.y,
			(float)viewport.resolution.x / (float)viewport.resolution.y,
			nearPlane, farPlane
		);
	}

	glm::mat4 getViewMatrix(const CameraComponent & cam, const ViewportComponent & viewport) {
		const auto facings = cameraHelper::getFacings(cam);
		const auto camPos = toVec(cam.frustum.position);
		return glm::lookAt(camPos, camPos + toVec(facings.front), toVec(facings.up));
	}
}
