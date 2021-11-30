#pragma once

#include "UniquePtr.hpp"

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

namespace kengine {
	using NavMeshPtr = UniquePtr<dtNavMesh, dtFreeNavMesh>;
	using NavMeshQueryPtr = UniquePtr<dtNavMeshQuery, dtFreeNavMeshQuery>;

	struct RecastNavMeshComponent {
		NavMeshPtr navMesh = nullptr;
		NavMeshQueryPtr navMeshQuery = nullptr;
	};
}