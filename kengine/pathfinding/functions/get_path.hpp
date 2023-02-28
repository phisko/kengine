#pragma once

// putils
#include "putils/point.hpp"
#include "putils/vector.hpp"

// kengine
#include "kengine/base_function.hpp"

#ifndef KENGINE_PATHFINDING_NAV_MESH_MAX_PATH_LENGTH
#define KENGINE_PATHFINDING_NAV_MESH_MAX_PATH_LENGTH 128
#endif

namespace kengine::pathfinding {
	namespace get_path_impl {
		static constexpr char path_name[] = "nav_mesh_path";
		using path = putils::vector<putils::point3f, KENGINE_PATHFINDING_NAV_MESH_MAX_PATH_LENGTH, path_name>;
	}

	// `environment` is the entity instantiating the `model Entity` this component is attached to
	using get_path_signature = get_path_impl::path(entt::handle environment, const putils::point3f & start, const putils::point3f & end);

	//! putils reflect all
	//! parents: [refltype::base]
	//! used_types: [kengine::pathfinding::get_path_impl::path, putils::point3f]
	struct get_path : base_function<get_path_signature> {};
}

#include "get_path.rpp"