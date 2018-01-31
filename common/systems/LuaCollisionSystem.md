# [LuaCollisionSystem](LuaCollisionSystem.hpp)

`System` that listens for [Collision](../packets/Collision.hpp) packets and fills a table in the [LuaSystem](LuaSystem.md)'s state.

### Behavior

Whenever a `Collision` packet is received, the `LuaCollisionSystem` performs the following for each `GameObject` involved in the collision:

* If the `GameObject` has a [LuaComponent](../components/LuaComponent.md)
    * If its `LuaComponent`'s `meta` property has an `onCollision` field
        * Call `meta.onCollision(go, other)`, with `go` and `other` being the references to the two `GameObjects` involved.

