# [PathfinderComponent](PathfinderComponent.hpp)

`Component` that specifies the pathfinding information to be used by the [PathfinderSystem](../systems/PathfinderSystem.md).

### Members

##### dest

```cpp
putils::Point3d dest;
```
Destination for the `GameObject`.

##### reached

```cpp
bool reached = true;
```
Indicates whether the `GameObject` has reached its destination. Must be set to `false` for the `PathfinderSystem` to take the `GameObject` into account.

##### diagonals

```cpp
bool diagonals = true;
```
Indicates whether the `GameObject` can move in diagonals.

##### desiredDistance

```cpp
double desiredDistance = 1;
```
Distance from `dest` at which the `GameObject` should move.

##### maxAvoidance

```cpp
double maxAvoidance = std::numeric_limits<double>::max();
```
Indicates how far in the opposite direction the `GameObject` may consider moving to try and reach `dest`. If there is a possibility that no path will be found to reach `dest`, it is important to set this value relatively low in order to avoid the `PathfinderSystem` evaluating every possible (and non-usable) path.
