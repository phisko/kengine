# [on_collision](on_collision.hpp)

`Function component` used as a callback when two entities collide.

## Prototype

```cpp
void (entt::entity first, entt::entity second);
```

### Parameters

* `first` & `second`: the two entities that are colliding

## Usage

Physics systems (such as the [bullet system](../bullet/systems/bullet.md)) call this `function component` when collisions are detected.