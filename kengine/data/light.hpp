#pragma once

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/point.hpp"
#include "putils/color.hpp"
#include "putils/vector.hpp"

#ifndef KENGINE_MAX_CSM_COUNT
# define KENGINE_MAX_CSM_COUNT 5
#endif

namespace kengine::data {
	struct light {
		putils::normalized_color color;
		float diffuse_strength = 1.f;
		float specular_strength = .1f;
		bool cast_shadows = true;
		int shadow_pcf_samples = 1;
		int shadow_map_size = 1024;
        float shadow_map_max_bias = .1f;
        float shadow_map_min_bias = .01f;
	};

	struct dir_light : light {
		static constexpr char vector_name[] = "dir_light_cascade_vector";
		using vector = putils::vector<float, KENGINE_MAX_CSM_COUNT, vector_name>;

		putils::vec3f direction = { 0.f, -1.f, 0.f };
		float ambient_strength = .1f;
        float light_sphere_distance = 500.f;

		vector cascade_ends = { 50.f };
		float shadow_caster_max_distance = 100.f;
	};

	struct point_light : light {
		float attenuation_constant = 1.f;
		float attenuation_linear = .09f;
		float attenuation_quadratic = .032f;
	};

	struct spot_light : point_light {
		putils::vec3f direction = { 0.f, -1.f, 0.f };
		float cut_off = 1.f;
		float outer_cut_off = 1.2f;
	};
}

#define refltype kengine::data::light
putils_reflection_info{
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
putils_reflection_info{
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
putils_reflection_info{
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
putils_reflection_info{
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
