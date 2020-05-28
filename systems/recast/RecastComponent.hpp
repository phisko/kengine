#pragma once

#include <vector>
#include <memory>

#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>

#include "Entity.hpp"

template<typename T, void (*FreeFunc)(T *)>
struct Deleter {
	void operator()(T * ptr) { FreeFunc(ptr); }
};

template<typename T, void(*FreeFunc)(T *)>
using UniquePtr = std::unique_ptr<T, Deleter<T, FreeFunc>>;

using NavMeshPtr = UniquePtr<dtNavMesh, dtFreeNavMesh>;
using NavMeshQueryPtr = UniquePtr<dtNavMeshQuery, dtFreeNavMeshQuery>;
using CrowdPtr = UniquePtr<dtCrowd, dtFreeCrowd>;

namespace kengine {
	struct RecastNavMeshComponent {
		NavMeshPtr navMesh = nullptr;
		NavMeshQueryPtr navMeshQuery = nullptr;
	};

	struct RecastCrowdComponent {
		CrowdPtr crowd = nullptr;
	};

	struct RecastAgentComponent {
		int index = 0;
		Entity::ID crowd = Entity::INVALID_ID;
	};
}