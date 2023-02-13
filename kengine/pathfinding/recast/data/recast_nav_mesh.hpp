#pragma once

// recast
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

// putils
#include "putils/default_constructors.hpp"

// kengine pathfinding/recast
#include "kengine/pathfinding/recast/helpers/unique_ptr.hpp"

namespace kengine::data {
	using nav_mesh_ptr = unique_ptr<dtNavMesh, dtFreeNavMesh>;
	using nav_mesh_query_ptr = unique_ptr<dtNavMeshQuery, dtFreeNavMeshQuery>;

	//! kengine registration off
	struct nav_mesh_data {
		unique_ptr<void, dtFree> data = nullptr;
		int size = 0;
	};

	//! putils reflect name
	struct recast_nav_mesh {
		nav_mesh_data data;
		nav_mesh_ptr nav_mesh = nullptr;
		nav_mesh_query_ptr nav_mesh_query = nullptr;
	};
}

#include "recast_nav_mesh.rpp"