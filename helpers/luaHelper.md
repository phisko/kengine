# [luaHelper](luaHelper.hpp)

Helper functions to easily register new types and functions with the lua state.

## Members

These are defined in the `kengine::luaHelper` namespace.

### registerType

```cpp
template<typename ... Types>
void registerTypes() noexcept;
```

Registers [reflectible](../putils/reflection.md) types with the lua state.

### registerComponents

```cpp
template<typename ... Comps>
void registerComponents() noexcept;
```

Registers a [reflectible](../putils/reflection.md) type with the lua state as a `Component` (adding `Entity` member functions).

### registerFunction

```cpp
template<typename F>
void registerFunction(const char * name, F && func) noexcept;
```

Register a new function with the lua state.
