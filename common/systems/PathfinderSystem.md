# [PathfinderSystem](PathfinderSystem.hpp)

`System` that moves objects towards a destination according to the information found in their [PathfinderComponent](../components/PathfinderComponent.md).

### Behavior

At each step, the `PathfinderSystem` uses an [AStar](../../putils/AStar.md) algorithm to calculate the next step each `GameObject` should take to reach the destination specified in its `PathfinderComponent`.

If a [PhysicsSystem](PhysicsSystem.md) was added **before** the `PathfinderSystem`, it will use a [QuadTree](../../putils/QuadTree.md) to determine whether a `GameObject` can move to any given position.
