#ifdef KENGINE_GLM

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrixHelper.hpp"

#include "data/TransformComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"

#include "helpers/cameraHelper.hpp"

namespace kengine::matrixHelper {
	glm::vec3 toVec(const putils::Point3f & pos) noexcept {
		return { pos.x, pos.y, pos.z };
	}

	putils::Point3f getPosition(const glm::mat4 & mat) noexcept {
		return { mat[3][0], mat[3][1], mat[3][2] };
	}

	putils::Vector3f getScale(const glm::mat4 & mat) noexcept {
		return {
			putils::getLength(putils::Vector3f{ mat[0][0], mat[0][1], mat[0][2] }),
			putils::getLength(putils::Vector3f{ mat[1][0], mat[1][1], mat[1][2] }),
			putils::getLength(putils::Vector3f{ mat[2][0], mat[2][1], mat[2][2] })
		};
	}

	putils::Vector3f getRotation(const glm::mat4 & mat) noexcept {
		putils::Point3f ret;
		glm::extractEulerAngleYXZ(mat, ret.y, ret.x, ret.z);
		return ret;
	}

	putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) noexcept {
		const auto tmp = conversionMatrix * glm::vec4(toVec(pos), 1.f);
		return { tmp.x, tmp.y, tmp.z };
	}

	glm::mat4 getModelMatrix(const TransformComponent & transform, const TransformComponent * modelTransform) noexcept {
		glm::mat4 model(1.f);

		{ // Object
			model = glm::translate(model, toVec(transform.boundingBox.position));

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

		if (modelTransform != nullptr) { // Model
			model = glm::scale(model, toVec(modelTransform->boundingBox.size));

			model = glm::rotate(model,
				modelTransform->yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::rotate(model,
				modelTransform->pitch,
				{ 1.f, 0.f, 0.f }
			);
			model = glm::rotate(model,
				modelTransform->roll,
				{ 0.f, 0.f, 1.f }
			);

			model = glm::translate(model, toVec(modelTransform->boundingBox.position)); // Re-center
		}

		return model;
	}
}

#endif
