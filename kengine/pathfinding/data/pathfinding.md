# [pathfinding](pathfinding.hpp)

Component that performs pathfinding for the entity.

## Members

### environment

```cpp
entt::entity environment;
```
Entity in which we are navigating. It must be an [instance](instance.md) of a model with a [nav_mesh](nav_mesh.md).

### destination

```cpp
putils::point3f destination;
```
Destination (in world coordinates) for the pathfinding.

### search_distance

```cpp
float search_distance = 2.f;
```
Maximum distance `destination` can be from the navmesh. Increasing this will cost performance.

### max_speed

```cpp
float max_speed = 1.f;
```

Maximum speed at which the entity may move.