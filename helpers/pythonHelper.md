# [pythonHelper](pythonHelper.hpp)

Helper functions are also provided to easily register new types and functions with the Python state.

## Members

These are defined in the `kengine::pythonHelper` namespace.

### registerType

```cpp
template<typename ... Types>
void registerTypes() noexcept;
```

Registers [reflectible](../putils/reflection.md) types with the Python state.

### registerComponents

```cpp
template<typename ... Comps>
void registerComponents() noexcept;
```

Registers [reflectible](../putils/reflection.md) types with the Python state as `Components` (adding `Entity` member functions).


### registerFunction

```cpp
template<typename F>
void registerFunction(const char * name, F && func) noexcept;
```

Register a new function with the Python state.
