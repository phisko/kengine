# [register_with_script_languages](register_with_script_languages.hpp)

```cpp
template<bool IsComponent, typename... Comps>
void register_with_script_languages(entt::registry & r) noexcept;
```

Registers a set of types with all currently supported script languages. If `IsComponent` is true, functions are registered to manipulate the types as components.

```cpp
template<typename F>
void register_with_script_languages(const entt::registry & r, const char * name, F && func) noexcept;
```

Registers a function with all currently supported script languages.