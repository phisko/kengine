# [register_get](register_get.hpp)

```cpp
template<typename ... Comps>
void register_get(entt::registry & r) noexcept;
```

Registers a standard implementation of the [get](../../../meta/get.md) `meta component` for each type in `Comps`.