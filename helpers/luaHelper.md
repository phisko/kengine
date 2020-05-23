# [luaHelper](luaHelper.hpp)

Helper functions to easily register new types and functions with the lua state.

## Members

These are defined in the `kengine::luaHelper` namespace.

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
