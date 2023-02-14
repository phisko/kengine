# [create_all_systems](create_all_systems.hpp)

```cpp
void create_all_systems(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
```

Creates all known systems in `destination_registry` by calling the [create_system](../functions/create_system.md) function components found in `main_registry`.

If `main_registry` is `nullptr`, it will be set to `destination_registry`.