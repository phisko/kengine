# [register_attributes](register_attributes.hpp)

```cpp
template<typename ... Comps>
void register_attributes(entt::registry & r) noexcept;
```

Registers a standard implementation of the [attributes](../../../meta/attributes.md) `meta component` for each type in `Comps`.