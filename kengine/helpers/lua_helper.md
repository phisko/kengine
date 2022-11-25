# [lua_helper](lua_helper.hpp)

Helper functions to easily register new types and functions with the Lua state.

## Members

These are defined in the `kengine::lua_helper` namespace.

### register_types

```cpp
template<typename ... Types>
void register_types(const entt::registry & r) noexcept;
```

Registers [reflectible](https://github.com/phisko/reflection) types with the lua state.

### register_components

```cpp
template<typename ... Comps>
void register_components(const entt::registry & r) noexcept;
```

Registers a [reflectible](https://github.com/phisko/reflection) type with the lua state as a component (adding entity member functions).

### register_function

```cpp
template<typename F>
void register_function(const entt::registry & r, const char * name, F && func) noexcept;
```

Register a new function with the lua state.
