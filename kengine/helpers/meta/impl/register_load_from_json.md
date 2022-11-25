# [register_load_from_json](register_load_from_json.hpp)

```cpp
template<typename ... Comps>
void register_load_from_json(entt::registry & r) noexcept;
```

Registers a standard implementation of the [load_from_json](../../../meta/load_from_json.md) `meta component` for each type in `Comps`.