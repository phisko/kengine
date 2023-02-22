# [display_entity](display_entity.hpp)

```cpp
void display_entity(entt::const_handle e) noexcept;
```

Displays an entity as an ImGui tree with read-only attributes.

For components to appear in the ImGui tree, the [display](../functions/display.md) `meta component` must first have been registered for them, along with the basic [has](../../functions/has.md).