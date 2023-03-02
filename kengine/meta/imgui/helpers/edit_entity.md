# [edit_entity](edit_entity.hpp)

```cpp
bool edit_entity(entt::handle e) noexcept;
```

Displays an entity as an ImGui tree with write-enabled attributes. Returns whether a component was modified.

For components to appear in the ImGui tree, the [edit](../functions/edit.md) `meta component` must first have been registered for them, along with the basic [has](../../functions/has.md), [emplace_or_replace](../../functions/emplace_or_replace.md) and [remove](../../functions/remove.md).