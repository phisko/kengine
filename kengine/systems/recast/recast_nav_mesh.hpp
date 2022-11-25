#pragma once

// recast
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

// impl
#include "unique_ptr.hpp"

namespace kengine::data {
	using nav_mesh_ptr = unique_ptr<dtNavMesh, dtFreeNavMesh>;
	using nav_mesh_query_ptr = unique_ptr<dtNavMeshQuery, dtFreeNavMeshQuery>;

	struct recast_nav_mesh {
		nav_mesh_ptr nav_mesh = nullptr;
		nav_mesh_query_ptr nav_mesh_query = nullptr;
	};
}