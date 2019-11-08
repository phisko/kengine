#pragma once

#include "reflection.hpp"

namespace kengine {
	struct GodRaysComponent {
		float scattering = .1f;
		float nbSteps = 10.f;
		float defaultStepLength = 25.f;
		float intensity = 2.5f;

		pmeta_get_class_name(GodRaysComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysComponent::scattering),
			pmeta_reflectible_attribute(&GodRaysComponent::nbSteps),
			pmeta_reflectible_attribute(&GodRaysComponent::defaultStepLength),
			pmeta_reflectible_attribute(&GodRaysComponent::intensity)
		);
	};
}