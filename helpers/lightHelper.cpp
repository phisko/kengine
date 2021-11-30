#ifdef PUTILS_GLM

#include "lightHelper.hpp"

namespace kengine::lightHelper {
	static glm::vec3 toVec(const putils::Vector3f & p) noexcept {
		return { p.x, p.y, p.z };
	}

	static glm::vec3 getCorrectDirection(const glm::vec3 & dir) noexcept {
		const auto normalized = glm::normalize(dir);
		return normalized.y == 1.f ?
			glm::vec3(.001f, .999f, 0.f) :
			normalized.y == -1.f ?
			glm::vec3(.001f, -.999f, 0.f) :
			dir;
	}

	float getCSMCascadeEnd(const DirLightComponent & light, size_t csmIndex) noexcept {
		return light.cascadeEnds[csmIndex];
	}

	glm::mat4 getCSMLightSpaceMatrix(const DirLightComponent & light, const putils::gl::Program::Parameters & params, size_t csmIndex) noexcept {
		const float ar = (float)params.viewport.size.x / (float)params.viewport.size.y;
		const float tanHalfHFOV = std::tan(params.camFOV * ar / 2.f);
		const float tanHalfVFOV = std::tan(params.camFOV / 2.f);

		const float cascadeStart = csmIndex == 0 ? SHADOW_MAP_NEAR_PLANE : getCSMCascadeEnd(light, csmIndex - 1);
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

		for (size_t i = 0; i < putils::lengthof(frustumCorners); ++i) {
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

	glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, const putils::gl::Program::Parameters & params) noexcept {
		const auto lightProjection = glm::perspective(45.f, (float)params.viewport.size.x / (float)params.viewport.size.y, SHADOW_MAP_NEAR_PLANE, SHADOW_MAP_FAR_PLANE);

		const auto dir = getCorrectDirection(toVec(light.direction));
		const auto lightView = glm::lookAt(pos, pos + dir, glm::vec3(0.f, 1.f, 0.f));

		return lightProjection * lightView;
	}

	float getRadius(const PointLightComponent & light) noexcept {
		const auto maxChannel = std::fmax(std::fmax(light.color.r, light.color.g), light.color.b);
		return (-light.linear + std::sqrtf(light.linear * light.linear -
			4.f * light.quadratic * (light.constant - 256.f * maxChannel * light.diffuseStrength)))
			/
			(2.f * light.quadratic);
	}
}

#endif