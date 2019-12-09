#pragma once

#include "reflection.hpp"

namespace kengine {
	struct GodRaysComponent {
		float scattering = .1f;
		float nbSteps = 10.f;
		float defaultStepLength = 25.f;
		float intensity = 2.5f;

		putils_reflection_class_name(GodRaysComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&GodRaysComponent::scattering),
			putils_reflection_attribute(&GodRaysComponent::nbSteps),
			putils_reflection_attribute(&GodRaysComponent::defaultStepLength),
			putils_reflection_attribute(&GodRaysComponent::intensity)
		);
	};
}