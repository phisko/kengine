# [edit_imgui](edit_imgui.hpp)

`Meta component` that displays the parent component as an `ImGui` tree with write-enabled attributes.

## Prototype

```cpp
void (entt::handle e);
```

### Parameters

* `e`: entity for which the parent component should be edited

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to edit.

A helper [register_edit_imgui](../helpers/impl/register_edit_imgui.md) function is provided that takes as a template parameter a set of component types and implements this `meta component` for them.

Note that the implementation provided in `register_edit_imgui` is only a sample, and users may freely replace it with any other implementation they desire.

The [imgui_helper::edit_entity](../helpers/imgui_helper.md) function calls this `meta component` to display entities.