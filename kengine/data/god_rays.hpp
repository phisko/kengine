#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct god_rays {
		float scattering = .1f;
		int nb_steps = 10;
		float default_step_length = 25.f;
		float intensity = 2.5f;
	};
}

#define refltype kengine::data::god_rays
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scattering),
		putils_reflection_attribute(nb_steps),
		putils_reflection_attribute(default_step_length),
		putils_reflection_attribute(intensity)
	);
};
#undef refltype