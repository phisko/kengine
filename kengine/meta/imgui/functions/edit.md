# [edit](edit.hpp)

`Meta component` that displays the parent component as an `ImGui` tree with write-enabled attributes.

## Prototype

```cpp
bool (entt::handle e);
```

### Parameters

* `e`: entity for which the parent component should be edited

### Return value

Whether the component was modified.

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to edit.

A [standard implementation](../helpers/impl/edit.md) is provided.

Note that the implementation is only a sample, and users may freely replace it with any other implementation they desire.

The [edit_entity](../../imgui/helpers/edit_entity.md) function calls this `meta component` to display entities.