#pragma once

#ifdef KENGINE_GLM

// glm
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

// putils
#include "Point.hpp"

namespace kengine {
	struct TransformComponent;
	struct CameraComponent;
	struct ViewportComponent;

	namespace matrixHelper {
		KENGINE_CORE_EXPORT glm::vec3 toVec(const putils::Point3f & pos) noexcept;

		KENGINE_CORE_EXPORT putils::Point3f getPosition(const glm::mat4 & mat) noexcept;
		KENGINE_CORE_EXPORT putils::Vector3f getScale(const glm::mat4 & mat) noexcept;
		KENGINE_CORE_EXPORT putils::Vector3f getRotation(const glm::mat4 & mat) noexcept;

		KENGINE_CORE_EXPORT putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) noexcept;

		KENGINE_CORE_EXPORT glm::mat4 getModelMatrix(const TransformComponent & transform, const TransformComponent * modelTransform = nullptr) noexcept;
	}
}

#endif