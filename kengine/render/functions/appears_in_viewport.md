# [appears_in_viewport](appears_in_viewport.hpp)

`Function component` used to determine whether entities should appear in [viewports](../data/viewport.md).

## Prototype

```cpp
bool (entt::entity entity);
```

If attached to a viewport, the function will be called for each entity that might be drawn into the viewport.

If attached to a non-viewport entity, the function will be called for each viewport it might be drawn into.

## Usage

Graphics systems may call this `function component` to know whether a given shader should be drawn in a viewport.

Shaders may call this `function component` to know whether an entity should appear in a viewport.