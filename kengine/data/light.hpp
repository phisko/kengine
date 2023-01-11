#pragma once

// putils
#include "putils/point.hpp"
#include "putils/color.hpp"
#include "putils/vector.hpp"

#ifndef KENGINE_MAX_CSM_COUNT
#define KENGINE_MAX_CSM_COUNT 5
#endif

namespace kengine::data {
	//! putils reflect all
	//! used_types: [putils::normalized_color]
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

	//! putils reflect all
	//! parents: [kengine::data::light]
	//! used_types: [refltype::vector, putils::vec3f]
	struct dir_light : light {
		static constexpr char vector_name[] = "dir_light_cascade_vector";
		using vector = putils::vector<float, KENGINE_MAX_CSM_COUNT, vector_name>;

		putils::vec3f direction = { 0.f, -1.f, 0.f };
		float ambient_strength = .1f;
		float light_sphere_distance = 500.f;

		vector cascade_ends = { 50.f };
		float shadow_caster_max_distance = 100.f;
	};

	//! putils reflect all
	//! parents: [kengine::data::light]
	struct point_light : light {
		float attenuation_constant = 1.f;
		float attenuation_linear = .09f;
		float attenuation_quadratic = .032f;
	};

	//! putils reflect all
	//! parents: [kengine::data::point_light]
	//! used_types: [putils::vec3f]
	struct spot_light : point_light {
		putils::vec3f direction = { 0.f, -1.f, 0.f };
		float cut_off = 1.f;
		float outer_cut_off = 1.2f;
	};
}

#include "light.reflection.hpp"