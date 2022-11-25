# [display_imgui](display_imgui.hpp)

`Meta component` that displays the parent component as an `ImGui` tree with read-only attributes.

## Prototype

```cpp
void (entt::const_handle e);
```

### Parameters

* `e`: entity for which the parent component should be displayed

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to display.

A helper [register_display_imgui](../helpers/meta/impl/register_display_imgui.md) function is provided that takes as a template parameter a set of component types and implements this `meta component` for them.

Note that the implementation provided in `register_display_imgui` is only a sample, and users may freely replace it with any other implementation they desire.

The [imgui_helper::display_entity](../helpers/imgui_helper.md) function calls this `meta component` to display entities.