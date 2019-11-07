#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "components/LightComponent.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;

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

	static float getCSMCascadeEnd(const DirLightComponent & light, size_t csmIndex) {
		return light.cascadeEnds[csmIndex];
	}

	static glm::mat4 getCSMLightSpaceMatrix(const DirLightComponent & light, const putils::gl::Program::Parameters & params, size_t csmIndex) {
		const float ar = (float)params.viewPort.size.x / (float)params.viewPort.size.y;
		const float tanHalfHFOV = std::tan(params.camFOV * ar / 2.f);
		const float tanHalfVFOV = std::tan(params.camFOV / 2.f);

		const float cascadeStart = csmIndex == 0 ? params.nearPlane : getCSMCascadeEnd(light, csmIndex - 1);
		const float cascadeEnd = getCSMCascadeEnd(light, csmIndex);

		const float xn = cascadeStart * tanHalfHFOV;
		const float yn = cascadeStart * tanHalfVFOV;

		const float xf = cascadeEnd * tanHalfHFOV;
		const float yf = cascadeEnd * tanHalfVFOV;

		const glm::vec3 frustumCorners[] = {
			// near face
			{ xn, yn, -cascadeStart },
			{ -xn, yn, -cascadeStart },
			{ xn, -yn, -cascadeStart },
			{ -xn, -yn, -cascadeStart },

			// far face
			{ xf, yf, -cascadeEnd },
			{ -xf, yf, -cascadeEnd },
			{ xf, -yf, -cascadeEnd },
			{ -xf, -yf, -cascadeEnd }
		};

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		const auto dir = getCorrectDirection(toVec(light.direction));

		const auto lightView = glm::lookAt({ 0.f, 0.f, 0.f }, dir, { 0.f, 1.f, 0.f });
		const auto inverseView = glm::inverse(params.view);

		for (size_t i = 0; i < lengthof(frustumCorners); ++i) {
			const auto worldPos = inverseView * glm::vec4(frustumCorners[i], 1.f);
			const auto lightPos = lightView * worldPos;

			minX = std::min(minX, lightPos.x);
			maxX = std::max(maxX, lightPos.x);
			minY = std::min(minY, lightPos.y);
			maxY = std::max(maxY, lightPos.y);
			minZ = std::min(minZ, lightPos.z);
			maxZ = std::max(maxZ, lightPos.z);
		}

		const auto lightProj = glm::ortho(minX, maxX, minY, maxY, minZ - light.shadowCasterMaxDistance, maxZ + light.shadowCasterMaxDistance);
		return lightProj * lightView;
	}

	static glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, const putils::gl::Program::Parameters & params) {
		const auto lightProjection = glm::perspective(45.f, (float)params.viewPort.size.x / (float)params.viewPort.size.y, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);

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