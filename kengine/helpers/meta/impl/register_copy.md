# [register_copy](register_copy.hpp)

```cpp
template<typename ... Comps>
void register_copy(entt::registry & r) noexcept;
```

Registers a standard implementation of the [copy](../../../meta/copy.md) `meta component` for each type in `Comps`.