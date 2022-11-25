# [register_has](register_has.hpp)

```cpp
template<typename ... Comps>
void register_has(entt::registry & r) noexcept;
```

Registers a standard implementation of the [has](../../../meta/has.md) `meta component` for each type in `Comps`.