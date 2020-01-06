# [EditImGui](EditImGui.hpp)

`Meta Component` that displays the parent `Component` as an `ImGui` tree with write-enabled attributes.

## Prototype

```cpp
void (const Entity & e);
```

### Parameters

* `e`: `Entity` for which the parent `Component` should be edited

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to edit.

A helper [registerComponentEditor](../../helpers/RegisterComponentEditor.md) function is provided that takes as a template parameter a `Component` type and implements the [DisplayImGui](DisplayImGui.md) and `EditImGui` `meta Components` for it.

Note that the implementation provided in `registerComponentEditor` is only a sample, and users may freely replace it with any other implementation they desire.

The [ImGuiHelper::editEntity](../../helpers/ImGuiHelper.md) function calls this `meta Component` to display `Entities`.