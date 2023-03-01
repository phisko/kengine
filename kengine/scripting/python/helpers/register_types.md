# [python_helper](python_helper.hpp)

Helper functions are also provided to easily register new types and functions with the Python state.

## Members

These are defined in the `kengine::python_helper` namespace.

### register_types

```cpp
template<bool IsComponent, typename... Types>
void register_types(entt::registry & r) noexcept;
```

Registers [reflectible](https://github.com/phisko/reflection) types with the Python state. If `IsComponent` is `true`, functions are also registered to manipulate the types as components.

### register_function

```cpp
template<typename F>
void register_function(const char * name, F && func) noexcept;
```

Register a new function with the Python state.
