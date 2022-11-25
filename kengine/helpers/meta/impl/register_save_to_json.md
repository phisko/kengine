# [register_save_to_json](register_save_to_json.hpp)

```cpp
template<typename ... Comps>
void register_save_to_json(entt::registry & r) noexcept;
```

Registers a standard implementation of the [save_to_json](../../../meta/save_to_json.md) `meta component` for each type in `Comps`.