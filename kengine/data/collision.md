# [collision](collision.hpp)

Component used to react when the entity collides with another.

## Members

### on_collide

```cpp
putils::function<void(entt::entity, entt::entity), KENGINE_COLLISION_FUNCTION_SIZE> on_collide;
```

Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_COLLISION_FUNCTION_SIZE` macro.