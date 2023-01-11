#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::god_rays
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scattering),
		putils_reflection_attribute(nb_steps),
		putils_reflection_attribute(default_step_length),
		putils_reflection_attribute(intensity)
	);
};
#undef refltype