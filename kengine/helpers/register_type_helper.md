# [register_type_helper](register_type_helper.hpp)

Helper functions to quickly register all currently implemented type utilities (`meta components`, bindings for scripting languages) for a set of types. These functions will not compile if all the functionalities they apply to are not activated (e.g. Lua bindings, Python bindings...)

## Members

### register_types

```cpp
template<typename ... Types>
void register_types(entt::registry & r) noexcept;
```

Registers a set of non-component types. For each `T` in `Types`:

* calls [`lua::register_type<T>()`](lua_helper.md)
* calls [`python::register_type<T>()`](python_helper.md)
* calls `register_types<U>()` for each `U` in `T`'s [used types](https://github.com/phisko/reflection)

### register_components

```cpp
template<typename ... Comps>
void register_components(entt::registry & r) noexcept;
```

Calls `register_types<Comps...>()`, registers all the existing [meta components](../meta) and adds a [name component](../data/name.md) to the `type entities` for `Comps`.

### register_function

```cpp
template<typename F>
void register_function(entt::registry & r, const char * name, F && func) noexcept;
```

Registers a function with all scripting languages (Lua and Python).
