# [register_types](register_types.hpp)

```cpp
template<bool IsComponent, typename... Types>
void register_types(const entt::registry & r) noexcept;
```

Registers [reflectible](https://github.com/phisko/reflection) types with the Lua state. If `IsComponent` is `true`, functions are also registered to manipulate the types as components.