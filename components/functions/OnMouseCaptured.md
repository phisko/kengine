# [OnMouseCaptured](OnMouseCaptured.hpp)

`Function Component` used to indicate that the mouse should be captured or released by a [window](../data/WindowComponent.md).

## Prototype

```cpp
void (EntityID window, bool captured);
```

### Parameters

* `window`: ID of the [window Entity](../data/WindowComponent.md) that should capture the mouse
* `captured`: whether the mouse should be captured or released

## Usage

This `function Component` is implemented in graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)) to perform the actual mouse capture.

It may also be implemented in user `systems` to be used as a callback, so that they may be notified when the mouse is captured.

Finally, it is up to the user code to actually call this `function Component` to indicate whether the application wants to capture the mouse or not.