# [PhysicsComponent](PhysicsComponent.hpp)

`Component` that specifies movement information about an `Entity`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### Constructor

```cpp
PhysicsComponent(bool solid = true, bool fixed = false);
```

##### solid

```cpp
bool solid = true;
```
Defines whether objects should collide with other objects.

##### fixed

```cpp
bool fixed = false;
```
Used to specify that the object will never move.

##### movement

```cpp
putils::Vector3f movement;
```
Indicates the movement vector for the entity.

##### speed

```cpp
double speed = 1;
```
Indicates a factor for the `movement` vector's properties.
