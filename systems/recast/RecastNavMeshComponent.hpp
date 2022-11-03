#pragma once

// recast
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

// impl
#include "UniquePtr.hpp"

namespace kengine {
	using NavMeshPtr = UniquePtr<dtNavMesh, dtFreeNavMesh>;
	using NavMeshQueryPtr = UniquePtr<dtNavMeshQuery, dtFreeNavMeshQuery>;

	struct RecastNavMeshComponent {
		NavMeshPtr navMesh = nullptr;
		NavMeshQueryPtr navMeshQuery = nullptr;
	};
}