# [PhysicsComponent](PhysicsComponent.hpp)

`Component` that specifies the movement information to be used by the [PhysicsSystem](../systems/PhysicsSystem.md).

### Members

##### Constructor

```cpp
PhysicsComponent(bool solid = true, bool fixed = false);
```

##### solid

```cpp
bool solid = true;
```
Can be used by client code to differentiate between solid and passthrough objects when handling [Collisions](../packets/Collision.hpp).

##### fixed

```cpp
bool fixed = false;
```
Used to specify that the object will never move, letting the `PhysicsSystem` skip the object.

##### movement

```cpp
putils::Point3d movement;
```
Indicates the movement vector for the entity.

##### speed

```cpp
double speed = 1;
```
Indicates a factor for the `movement` vector's properties.
