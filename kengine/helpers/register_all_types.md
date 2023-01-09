# [register_all_types](register_all_types.hpp)

```cpp
void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
```

Registers all known types with `destination_registry`:
* calls [register_everything](meta/register_everything.md) for all engine types
* calls all [register_types](../functions/register_types.md) function components found in `main_registry`

If `main_registry` is `nullptr`, it will be set to `destination_registry`.