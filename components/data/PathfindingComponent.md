# [PathfindingComponent](PathfindingComponent.hpp)

`Component` that specifies pathfinding parameters for an `Entity`.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD), but `environment` will be invalidated
* Processed by pathfinding systems (such as the [RecastSystem](../../systems/bullet/RecastSystem.md)).

## Members

### environment

```cpp
Entity::ID environment;
```
`Entity` in which we are navigating. It must be an [instance](InstanceComponent.md) of a model with a [NavMeshComponent](NavMeshComponent.md).

### destination

```cpp
putils::Point3f destination;
```
Destination (in world coordinates) for the pathfinding.

### searchDistance

```cpp
float searchDistance = 2.f;
```
Maximum distance `destination` can be from the navmesh. Increasing this will cost performance.