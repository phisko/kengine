# [LuaCollisionSystem](LuaCollisionSystem.hpp)

`System` that listens for [Collision](../packets/Collision.hpp) packets and fills a table in the [LuaSystem](LuaSystem.md)'s state.

### Behavior

##### *"collisions"* table

Whenever a `Collision` packet is received, the `LuaCollisionSystem` adds an entry to the ***"collisions"*** table in the `LuaSystem`'s state.

After receiving a `Collision` between two entities *"A"* and *"B"*, the following will be true:

```lua
collisions["A"] == "B"
collisions["B"] == "A"
```

The table is never purged or reset. Once a collision is processed, scripts should remove it from the table.

##### Collision handlers

Whenever a `Collision` packet is received, the `LuaCollisionSystem` also looks for a `collis
