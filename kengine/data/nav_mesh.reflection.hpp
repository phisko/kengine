#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::nav_mesh
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(concerned_mesh),
		putils_reflection_attribute(cell_size),
		putils_reflection_attribute(cell_height),
		putils_reflection_attribute(walkable_slope),
		putils_reflection_attribute(character_height),
		putils_reflection_attribute(character_climb),
		putils_reflection_attribute(character_radius),
		putils_reflection_attribute(max_edge_length),
		putils_reflection_attribute(max_simplification_error),
		putils_reflection_attribute(min_region_area),
		putils_reflection_attribute(merge_region_area),
		putils_reflection_attribute(verts_per_poly),
		putils_reflection_attribute(detail_sample_dist),
		putils_reflection_attribute(detail_sample_max_error),
		putils_reflection_attribute(query_max_search_nodes)
	);
};
#undef refltype

#define refltype kengine::functions::get_path
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
	putils_reflection_used_types(
		putils_reflection_type(kengine::functions::get_path_impl::path),
		putils_reflection_type(putils::point3f)
	);
};
#undef refltype