# [RecastSystem](RecastSystem.hpp)

System that creates navmeshes for `Entities` with [NavMeshComponents](../../components/data/NavMeshComponent.md) and stores them in [RecastNavMeshComponents](RecastNavMeshComponent.md).

These navmeshes are then used to perform pathfinding for `Entities` with [PathfindingComponents](../../components/data/PathfindingComponent.md).

The system also provides an OpenGL shader to debug navmeshes.