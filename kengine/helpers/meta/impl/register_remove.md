# [register_remove](register_remove.hpp)

```cpp
template<typename ... Comps>
void register_remove(entt::registry & r) noexcept;
```

Registers a standard implementation of the [remove](../../../meta/remove.md) `meta component` for each type in `Comps`.