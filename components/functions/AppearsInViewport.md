# [AppearsInViewport](AppearsInViewport.hpp)

`Function Component` returning whether the parent `Entity` should appear in a given viewport.

## Prototype

```cpp
bool (EntityID viewport);
```

### Parameters

* `viewport`: viewport for which we need to know whether to draw the parent `Entity` or not

## Usage

Graphics systems may call this `function Component` to know whether a given shader should be drawn in a viewport.

Shaders may call this `function Component` to know whether an `Entity` should appear in a viewport.