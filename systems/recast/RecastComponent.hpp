#pragma once

#include <vector>
#include <memory>

#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>

template<typename T, void (*FreeFunc)(T *)>
struct Deleter {
	void operator()(T * ptr) { FreeFunc(ptr); }
};

template<typename T, void(*FreeFunc)(T *)>
using UniquePtr = std::unique_ptr<T, Deleter<T, FreeFunc>>;

using PolyMeshPtr = UniquePtr<rcPolyMesh, rcFreePolyMesh>;
using PolyMeshDetailPtr = UniquePtr<rcPolyMeshDetail, rcFreePolyMeshDetail>;
using NavMeshPtr = UniquePtr<dtNavMesh, dtFreeNavMesh>;
using NavMeshQueryPtr = UniquePtr<dtNavMeshQuery, dtFreeNavMeshQuery>;

namespace kengine {
	struct RecastComponent {
		struct Mesh {
			PolyMeshPtr polyMesh;
			PolyMeshDetailPtr polyMeshDetail;
			NavMeshPtr navMesh;
			NavMeshQueryPtr navMeshQuery;
		};

		std::vector<Mesh> meshes;
	};
}