# [register_all_types](register_all_types.hpp)

## Members

### register_all_types

```cpp
void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
```

Registers all known types with `destination_registry` by calling all [register_types](../functions/register_types.md) function components found in `main_registry`.

If `main_registry` is `nullptr`, it will be set to `destination_registry`.

### pre_register_all_types

```cpp
void pre_register_all_types(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
```

"Pre-registers" all known types with `destination_registry`. This is useful if `register_all_types` is going to be called from a thread other than the main thread, resulting in potential concurrent creation of component pools, [type entities](type_helper.md), or other shared state.