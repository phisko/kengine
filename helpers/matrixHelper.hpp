#pragma once

#ifdef PUTILS_GLM

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "Point.hpp"

namespace kengine {
	struct TransformComponent;
	struct CameraComponent;
	struct ViewportComponent;

	namespace matrixHelper {
		glm::vec3 toVec(const putils::Point3f & pos) noexcept;

		putils::Point3f getPosition(const glm::mat4 & mat) noexcept;
		putils::Vector3f getScale(const glm::mat4 & mat) noexcept;
		putils::Vector3f getRotation(const glm::mat4 & mat) noexcept;

		putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) noexcept;

		glm::mat4 getModelMatrix(const TransformComponent & transform, const TransformComponent * modelTransform = nullptr) noexcept;

		glm::mat4 getProjMatrix(const CameraComponent & cam, const ViewportComponent & viewport, float nearPlane, float farPlane) noexcept;
		glm::mat4 getViewMatrix(const CameraComponent & cam, const ViewportComponent & viewport) noexcept;
	}
}

#endif