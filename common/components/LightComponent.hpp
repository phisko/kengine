#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace kengine {
	struct LightComponent {
		glm::vec3 color = { 1.f, 1.f, 1.f };
		float diffuseStrength = 1.f;
		float specularStrength = .1f;
	};

	struct DirLightComponent : LightComponent {
		glm::vec3 direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;

		pmeta_get_class_name(DirLightComponent);
	};

	struct PointLightComponent : LightComponent {
		float constant = 1.f;
		float linear = .09f;
		float quadratic = .032f;

		pmeta_get_class_name(PointLightComponent);
	};

	struct SpotLightComponent : PointLightComponent {
		glm::vec3 direction = { 0.f, -1.f, 0.f };
		float cutOff = glm::cos(glm::radians(12.5f));
		float outerCutOff = glm::cos(glm::radians(15.f));

		pmeta_get_class_name(SpotLightComponent);
	};

	static glm::vec3 getCorrectDirection(const glm::vec3 & dir) {
		const auto normalized = glm::normalize(dir);
		return normalized.y == 1.f ?
			glm::vec3(.001f, .999f, 0.f) :
			normalized.y == -1.f ?
			glm::vec3(.001f, -.999f, 0.f) :
			dir;
	}

	static auto DIRECTIONAL_LIGHT_SHADOW_DISTANCE = 100.f;

	static glm::mat4 getLightSpaceMatrix(const DirLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto dir = getCorrectDirection(light.direction);
		const auto dist = DIRECTIONAL_LIGHT_SHADOW_DISTANCE;
		const auto lightProjection = glm::ortho(pos.x - dist, pos.x + dist, pos.y - dist, pos.y + dist, pos.z - dist, pos.z + dist);
		const auto lightView = glm::lookAt(glm::vec3(0.f), dir, glm::vec3(0.f, 1.f, 0.f));
		return lightProjection * lightView;
	}

	static auto SHADOW_MAP_NEAR_PLANE = 0.f;
	static auto SHADOW_MAP_FAR_PLANE = 1000.f;

	static glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, size_t screenWidth, size_t screenHeight) {
		const auto lightProjection = glm::perspective(45.f, (float)screenWidth / (float)screenHeight, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);

		const auto dir = getCorrectDirection(light.direction);
		const glm::vec3 vPos(pos.x, pos.y, pos.z);
		const auto lightView = glm::lookAt(vPos, vPos + dir, glm::vec3(0.f, 1.f, 0.f));

		return lightProjection * lightView;
	}

	static float getRadius(const PointLightComponent & light) {
		const auto maxChannel = std::fmax(std::fmax(light.color.x, light.color.y), light.color.z);
		return (-light.linear + std::sqrtf(light.linear * light.linear -
			4.f * light.quadratic * (light.constant - 256.f * maxChannel)))
			/
			(2.f * light.quadratic);
	}
}
