# [imgui_helper](imgui_helper.hpp)

Helper functions for [ImGui](https://github.com/ocornut/imgui) + kengine operations.

## Usage

`display_entity` and `edit_entity` are implemented in terms of `meta components`.

For components to appear in the ImGui tree, the [display_imgui](../functions/display_imgui.md)/[edit_imgui](../functions/edit_imgui.md) `meta components` must first have been registered for them, along with the basic [has](../../functions/has.md), [emplace_or_replace](../../functions/emplace_or_replace.md) and [remove](../../functions/remove.md).

## Members

### display_entity

```cpp
void display_entity(entt::const_handle e) noexcept;
```

Displays an entity as an ImGui tree with read-only attributes.

### edit_entity

```cpp
void edit_entity(entt::handle e) noexcept;
```

Displays an entity as an ImGui tree with write-enabled attributes.

### get_scale

```cpp
float get_scale(const entt::registry & r) noexcept;
```

Returns the desired scale for all ImGui elements, calculated based on all existing [imgui_scale components](../../../imgui/data/scale.md).