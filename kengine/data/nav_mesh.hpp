#pragma once

#ifndef KENGINE_NAVMESH_MAX_PATH_LENGTH
#define KENGINE_NAVMESH_MAX_PATH_LENGTH 128
#endif

#ifndef KENGINE_NAVMESH_FUNC_SIZE
#define KENGINE_NAVMESH_FUNC_SIZE 64
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
	//! putils reflect all
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"
#endif

	// `environment` is the entity instantiating the `model Entity` this component is attached to
	using get_path_signature = get_path_impl::path(entt::handle environment, const putils::point3f & start, const putils::point3f & end);
	//! putils reflect all
	//! parents: [refltype::base]
	//! used_types: [kengine::functions::get_path_impl::path, putils::point3f]
	struct get_path : base_function<get_path_signature> {};

#ifdef __GNU_C__
#pragma GCC diagnostic pop
#endif
}

#include "nav_mesh.reflection.hpp"