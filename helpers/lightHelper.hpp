#pragma once

#ifndef PUTILS_GLM
static_assert(false, "lightHelper requires GLM");
#else
#include <glm/gtc/matrix_transform.hpp>
#include "data/LightComponent.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;

	namespace lightHelper {
		float getCSMCascadeEnd(const DirLightComponent & light, size_t csmIndex) noexcept;
		glm::mat4 getCSMLightSpaceMatrix(const DirLightComponent & light, const putils::gl::Program::Parameters & params, size_t csmIndex) noexcept;
		glm::mat4 getLightSpaceMatrix(const SpotLightComponent & light, const glm::vec3 & pos, const putils::gl::Program::Parameters & params) noexcept;
		float getRadius(const PointLightComponent & light) noexcept;
	}

	extern float SHADOW_MAP_NEAR_PLANE;
	extern float SHADOW_MAP_FAR_PLANE;
}
#endif