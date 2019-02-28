#include "LightHelper.hpp"
#include "components/LightComponent.hpp"

namespace kengine::LightHelper {
	glm::vec3 getCorrectDirection(const glm::vec3 & dir) {
		const auto normalized = glm::normalize(dir);
		return normalized.y == 1.f ?
			glm::vec3(.001f, .999f, 0.f) :
			normalized.y == -1.f ?
			glm::vec3(.001f, -.999f, 0.f) :
			dir;
	}

	glm::mat4 getLightSpaceMatrix(const DirLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto dir = getCorrectDirection(light.direction);
		const auto dist = DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
		const auto lightProjection = glm::ortho(pos.x - dist, pos.x + dist, pos.y - dist, pos.y + dist, pos.z - dist, pos.z + dist);
		const auto lightView = glm::lookAt(glm::vec3(0.f), dir, glm::vec3(0.f, 1.f, 0.f));
		return lightProjection * lightView;
	}

	glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto lightProjection = glm::perspective(45.f, (float)screenWidth / (float)screenHeight, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);

		const auto dir = getCorrectDirection(light.direction);
		const glm::vec3 vPos(pos.x, pos.y, pos.z);
		const auto lightView = glm::lookAt(vPos, vPos + dir, glm::vec3(0.f, 1.f, 0.f));

		return lightProjection * lightView;
	}

	float getRadius(const PointLightComponent & light) {
		const auto maxChannel = std::fmax(std::fmax(light.color.x, light.color.y), light.color.z);
		return (-light.linear + std::sqrtf(light.linear * light.linear -
			4.f * light.quadratic * (light.constant - 256.f * maxChannel * light.diffuseStrength)))
			/
			(2.f * light.quadratic);
	}
}