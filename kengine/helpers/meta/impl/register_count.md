# [register_count](register_count.hpp)

```cpp
template<typename ... Comps>
void register_count(entt::registry & r) noexcept;
```

Registers a standard implementation of the [count](../../../meta/count.md) `meta component` for each type in `Comps`.