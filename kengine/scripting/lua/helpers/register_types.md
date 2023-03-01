# [lua_helper](lua_helper.hpp)

Helper functions to easily register new types and functions with the Lua state.

## Members

These are defined in the `kengine::lua_helper` namespace.

### register_types

```cpp
template<bool IsComponent, typename... Types>
void register_types(const entt::registry & r) noexcept;
```

Registers [reflectible](https://github.com/phisko/reflection) types with the lua state. If `IsComponent` is `true`, functions are also registered to manipulate the types as components.

### register_function

```cpp
template<typename F>
void register_function(const entt::registry & r, const char * name, F && func) noexcept;
```

Register a new function with the lua state.
