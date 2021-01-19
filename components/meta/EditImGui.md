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

A helper [registerEditImGui](../../helpers/registerEditImGui.md) function is provided that takes as a template parameter a set of `Component` types and implements the `EditImGui` `meta Component` for them.

Note that the implementation provided in `registerEditImGui` is only a sample, and users may freely replace it with any other implementation they desire.

The [imguiHelper::editEntity](../../helpers/imguiHelper.md) function calls this `meta Component` to display `Entities`.