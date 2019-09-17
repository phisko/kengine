#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "components/LightComponent.hpp"

namespace kengine {
	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;

	extern float DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
	extern float SHADOW_MAP_NEAR_PLANE;
	extern float SHADOW_MAP_FAR_PLANE;
}

namespace kengine::LightHelper {
	static glm::vec3 toVec(const putils::Vector3f & p) { return { p.x, p.y, p.z }; }

	static glm::vec3 getCorrectDirection(const glm::vec3 & dir) {
		const auto normalized = glm::normalize(dir);
		return normalized.y == 1.f ?
			glm::vec3(.001f, .999f, 0.f) :
			normalized.y == -1.f ?
			glm::vec3(.001f, -.999f, 0.f) :
			dir;
	}

	static glm::mat4 getLightSpaceMatrix(const DirLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto dir = getCorrectDirection(toVec(light.direction));
		const auto dist = DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
		const auto lightProjection = glm::ortho(-dist, dist, -dist, dist, -dist, dist);
		const auto lightView = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));
		return lightProjection * lightView;
	}

	static glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto lightProjection = glm::perspective(45.f, (float)screenWidth / (float)screenHeight, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);

		const auto dir = getCorrectDirection(toVec(light.direction));
		const auto lightView = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));

		return lightProjection * lightView;
	}

	static float getRadius(const PointLightComponent & light) {
		const auto maxChannel = std::fmax(std::fmax(light.color.r, light.color.g), light.color.b);
		return (-light.linear + std::sqrtf(light.linear * light.linear -
			4.f * light.quadratic * (light.constant - 256.f * maxChannel * light.diffuseStrength)))
			/
			(2.f * light.quadratic);
	}
}