# [AppearsInViewport](AppearsInViewport.hpp)

`Function Component` used to determine whether `Entities` should appear in [Viewports](../data/ViewportComponent.md).

## Prototype

```cpp
bool (const Entity & entity);
```

If attached to a "viewport `Entity`", the function will be called for each `Entity` that might be drawn into the viewport.

If attached to a non-viewport `Entity`, the function will be called for each "viewport `Entity`" it might be drawn into.

## Usage

Graphics systems may call this `function Component` to know whether a given shader should be drawn in a viewport.

Shaders may call this `function Component` to know whether an `Entity` should appear in a viewport.