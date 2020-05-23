# [imguiHelper](imguiHelper.hpp)

Helper functions for [ImGui](https://github.com/ocornut/imgui) + Kengine operations.

## Members

### displayEntity

```cpp
void displayEntity(EntityManager & em, const Entity & e);
```

Displays an `Entity` as an ImGui tree with read-only attributes.

### editEntity

```cpp
void editEntity(EntityManager & em, Entity & e);
```

Displays an `Entity` as an ImGui tree with write-enabled attributes.

## Usage

`displayEntity` and `editEntity` are implemented in terms of `meta Components`.

For `Components` to appear in the ImGui tree, the [DisplayImGui](../components/meta/DisplayImGui.md)/[EditImGui](../components/meta/DisplayImGui.md) `meta Components` must first have been registered for them, along with the basic [Has](../components/meta/Has.md), [AttachTo](../components/meta/AttachTo.md) and [DetachFrom](../components/meta/DetachFrom.md).
