# [register_detach_from](register_detach_from.hpp)

```cpp
template<typename ... Comps>
void register_detach_from(entt::registry & r) noexcept;
```

Registers a standard implementation of the [detach_from](../../../meta/detach_from.md) `meta component` for each type in `Comps`.