#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "reflection/Reflectible.hpp"

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
}
