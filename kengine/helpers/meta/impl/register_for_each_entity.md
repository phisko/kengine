# [register_for_each_entity](register_for_each_entity.hpp)

```cpp
template<typename ... Comps>
void register_for_each_entity(entt::registry & r) noexcept;
```

Registers a standard implementation of the [for_each_entity](../../../meta/for_each_entity.md) and `for_each_entity_without` `meta components` for each type in `Comps`.