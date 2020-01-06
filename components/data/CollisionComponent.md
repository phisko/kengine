# [CollisionComponent](CollisionComponent.hpp)

`Component` that holds a callback called when the `Entity` it is attached to collides with another.


## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable as it contains a functor
* Processed by the [CollisionSystem](../../systems/CollisionSystem.md).

## Members

### onCollide

```cpp
putils::function<void(Entity & self, Entity & other), KENGINE_COLLISION_FUNCTION_SIZE> onCollide;
```

Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_COLLISION_FUNCTION_SIZE` macro.