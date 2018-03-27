# [LuaSystem](LuaSystem.hpp)

[ScriptSystem](../../ScriptSystem.md) that executes lua scripts, either at a global scale or attached to a `GameObject` through a [LuaComponent](../components/LuaComponent.md).

### Members

##### Constructor

If a *"scripts"* directory is found, `addScriptDirectory("scripts")` is called.

### Queries

In order to easily extend the `LuaSystem`'s behavior (cf. [LuaCollisionSystem](LuaCollisionSystem.md)), the lua global state can be obtained through a [LuaState::Query](../packets/LuaState.hpp).
