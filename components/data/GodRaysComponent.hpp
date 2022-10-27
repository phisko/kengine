#pragma once

#include "reflection.hpp"

namespace kengine {
	struct GodRaysComponent {
		float scattering = .1f;
		int nbSteps = 10;
		float defaultStepLength = 25.f;
		float intensity = 2.5f;
	};
}

#define refltype kengine::GodRaysComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scattering),
		putils_reflection_attribute(nbSteps),
		putils_reflection_attribute(defaultStepLength),
		putils_reflection_attribute(intensity)
	);
};
#undef refltype