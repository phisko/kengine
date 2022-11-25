# [register_attach_to](register_attach_to.hpp)

```cpp
template<typename ... Comps>
void register_attach_to(entt::registry & r) noexcept;
```

Registers a standard implementation of the [attach_to](../../../meta/attach_to.md) `meta component` for each type in `Comps`.