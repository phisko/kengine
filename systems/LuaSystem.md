# [LuaSystem](LuaSystem.hpp)

[ScriptSystem](ScriptSystem.md) that executes lua scripts attached to `Entities` through [LuaComponents](../components/LuaComponent.md).

Helper functions are also provided to easily register new types and functions with the lua state.

## Members

These are defined in the `kengine::lua` namespace.

### registerType

```cpp
template<typename T>
void registerType(EntityManager & em);
```

Registers a [reflectible](../putils/reflection.md) type with the lua state.

### registerFunction

```cpp
template<typename F>
void registerFunction(EntityManager & em, const char * name, F && func);
```

Register a new function with the lua state.