#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "Point.hpp"

namespace kengine {
	struct ModelComponent;
	struct TransformComponent;

	namespace matrixHelper {
		inline glm::vec3 toVec(const putils::Point3f & pos);

		inline putils::Point3f getPosition(const glm::mat4 & mat);
		inline putils::Vector3f getRotation(const glm::mat4 & mat);

		inline putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix);

		glm::mat4 getModelMatrix(const ModelComponent & model, const TransformComponent & transform);

		// Impl
		inline glm::vec3 toVec(const putils::Point3f & pos) {
			return { pos.x, pos.y, pos.z };
		}

		inline putils::Point3f getPosition(const glm::mat4 & mat) {
			return { mat[3][0], mat[3][1], mat[3][2] };
		}

		inline putils::Vector3f getRotation(const glm::mat4 & mat) {
			putils::Point3f ret;
			glm::extractEulerAngleXYZ(mat, ret.x, ret.y, ret.z);
			return ret;
		}

		inline putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) {
			const auto tmp = conversionMatrix * glm::vec4(toVec(pos), 1.f);
			return { tmp.x, tmp.y, tmp.z };
		}
	}
}