#pragma once

#ifndef KENGINE_NAVMESH_MAX_PATH_LENGTH
# define KENGINE_NAVMESH_MAX_PATH_LENGTH 128
#endif

#ifndef KENGINE_NAVMESH_FUNC_SIZE
# define KENGINE_NAVMESH_FUNC_SIZE 64
#endif

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/angle.hpp"
#include "putils/point.hpp"
#include "putils/vector.hpp"

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::data {
	struct nav_mesh {
		size_t concerned_mesh = 0; // Index into data::model_data::meshes, pointing to the mesh for which to generate the navmesh
		float cell_size = .25f;
		float cell_height = .25f;
		float walkable_slope = putils::pi / 4.f;
		float character_height = 1.f;
		float character_climb = .75f;
		float character_radius = .5f;
		int max_edge_length = 80;
		float max_simplification_error = 1.1f;
		float min_region_area = 9.f;
		float merge_region_area = 25.f;
		int verts_per_poly = 6;
		float detail_sample_dist = 75.f;
		float detail_sample_max_error = 20.f;
		int query_max_search_nodes = 65535;
	};
}

namespace kengine::functions {
	namespace get_path_impl {
		static constexpr char path_name[] = "nav_mesh_path";
		using path = putils::vector<putils::point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH, path_name>;
	}

#ifdef __GNUC__
// Ignore "'...' has a base '...' whose type uses the anonymous namespace" warnings
// Haven't found any type here that uses the anonymous namespace, not sure where this is coming from
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wsubobject-linkage"
#endif

		struct get_path : base_function<
			get_path_impl::path (entt::handle environment, const putils::point3f & start, const putils::point3f & end)
			// `environment` is the entity instantiating the `model Entity` this component is attached to
		> {};

#ifdef __GNU_C__
#	pragma GCC diagnostic pop
#endif
}

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
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
	putils_reflection_used_types(
		putils_reflection_type(kengine::functions::get_path_impl::path)
	);
};
#undef refltype