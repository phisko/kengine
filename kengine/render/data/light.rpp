#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::light
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(color),
		putils_reflection_attribute(diffuse_strength),
		putils_reflection_attribute(specular_strength),
		putils_reflection_attribute(cast_shadows),
		putils_reflection_attribute(shadow_pcf_samples),
		putils_reflection_attribute(shadow_map_size),
		putils_reflection_attribute(shadow_map_max_bias),
		putils_reflection_attribute(shadow_map_min_bias)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype

#define refltype kengine::data::dir_light
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(direction),
		putils_reflection_attribute(ambient_strength),
		putils_reflection_attribute(light_sphere_distance),
		putils_reflection_attribute(cascade_ends),
		putils_reflection_attribute(shadow_caster_max_distance)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::data::light)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::vector),
		putils_reflection_type(putils::vec3f)
	);
};
#undef refltype

#define refltype kengine::data::point_light
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(attenuation_constant),
		putils_reflection_attribute(attenuation_linear),
		putils_reflection_attribute(attenuation_quadratic)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::data::light)
	);
};
#undef refltype

#define refltype kengine::data::spot_light
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(direction),
		putils_reflection_attribute(cut_off),
		putils_reflection_attribute(outer_cut_off)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::data::point_light)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::vec3f)
	);
};
#undef refltype