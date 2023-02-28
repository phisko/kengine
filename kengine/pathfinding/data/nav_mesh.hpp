#pragma once

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

namespace kengine::pathfinding {
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

#include "nav_mesh.rpp"