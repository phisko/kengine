#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace kengine {
	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;

	extern float DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
	extern float SHADOW_MAP_NEAR_PLANE;
	extern float SHADOW_MAP_FAR_PLANE;
}

namespace kengine::LightHelper {
	glm::vec3 getCorrectDirection(const glm::vec3 & dir);

	glm::mat4 getLightSpaceMatrix(const DirLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight);
	glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight);

	float getRadius(const PointLightComponent & light);
}