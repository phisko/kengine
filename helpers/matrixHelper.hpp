#pragma once

#include <glm/glm.hpp>
#include "Point.hpp"

namespace kengine {
	struct ModelComponent;
	struct TransformComponent;

	namespace matrixHelper {
		inline glm::vec3 toVcec(const putils::Point3f & pos);
		inline putils::Point3f getPos(const glm::mat4 & mat);
		inline putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix);
		glm::mat4 getModelMatrix(const ModelComponent & model, const TransformComponent & transform);

		// Impl
		inline glm::vec3 toVec(const putils::Point3f & pos) {
			return { pos.x, pos.y, pos.z };
		}

		inline putils::Point3f getPos(const glm::mat4 & mat) {
			return { mat[3][0], mat[3][1], mat[3][2] };
		}

		inline putils::Point3f convertToReferencial(const putils::Point3f & pos, const glm::mat4 & conversionMatrix) {
			const auto tmp = conversionMatrix * glm::vec4(toVec(pos), 1.f);
			return { tmp.x, tmp.y, tmp.z };
		}
	}
}