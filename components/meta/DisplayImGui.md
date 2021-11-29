# [DisplayImGui](DisplayImGui.hpp)

`Meta Component` that displays the parent `Component` as an `ImGui` tree with read-only attributes.

## Prototype

```cpp
void (const Entity & e);
```

### Parameters

* `e`: `Entity` for which the parent `Component` should be displayed

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to display.

A helper [registerDisplayImGui](../../helpers/meta/impl/registerDisplayImGui.md) function is provided that takes as a template parameter a set of `Component` types and implements the `DisplayImGui` `meta Components` for them.

Note that the implementation provided in `registerDisplayImGui` is only a sample, and users may freely replace it with any other implementation they desire.

The [imguiHelper::displayEntity](../../helpers/imguiHelper.md) function calls this `meta Component` to display `Entities`.