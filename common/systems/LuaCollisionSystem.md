# [LuaCollisionSystem](LuaCollisionSystem.hpp)

`System` that listens for [Collision](../packets/Collision.hpp) packets and fills a table in the [LuaSystem](LuaSystem.md)'s state.

### Behavior

Upon construction, a global `collisionHandlers` table is definde in the `LuaSystem`'s state. This table can be populated by scripts by adding functions with the following prototype:

```lua
function (name, otherName) end
```

`name` and `otherName` are the names of the [GameObjects](../../GameObject.md) involved in the collision.

Whenever a `Collision` packet is received, the `LuaCollisionSystem` iterates over the `collisionHandlers` table and calls each handler it contains with the name of the two `GameObjects` involved.
