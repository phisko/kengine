# [CollisionComponent](CollisionComponent.hpp)

`Component` that specifies what should happen when the `Entity` it is attached to collides with another.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable

### Members

##### onCollide

```cpp
putils::function<void(kengine::Entity & self, kengine::Entity & other), KENGINE_COLLISION_FUNCTION_SIZE> onCollide;
```

Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_COLLISION_FUNCTION_SIZE` macro.