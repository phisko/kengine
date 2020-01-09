# [PhysicsComponent](PhysicsComponent.hpp)

`Component` that specifies movement information about an `Entity`.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md)).

## Members

### movement

```cpp
putils::Vector3f movement;
```
Indicates the movement vector for the entity in the current frame.

### yaw, pitch, roll

```cpp
float yaw = 0.f;
float pitch = 0.f;
float roll = 0.f;
```

Radians to turn in the current frame.

### mass

```cpp
float mass = 1.f;
```

### speed

```cpp
float speed = 1;
```
Indicates a factor for the `movement` vector's properties.

### changed

```cpp
bool changed = false;
```
Indicates that the component was modified by a "user" system (and not the actual physics system) and should be read by the physics system.