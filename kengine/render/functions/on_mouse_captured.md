# [on_mouse_captured](on_mouse_captured.hpp)

`Function component` that captures or releases the mouse by a [window](../data/window.md).

## Prototype

```cpp
void (entt::entity window, bool captured);
```

### Parameters

* `window`: [window](../data/window.md) that should capture the mouse
* `captured`: whether the mouse should be captured or released

## Usage

This `function component` is implemented in graphics systems to perform the actual mouse capture.

It may also be implemented in user systems to be used as a callback, so that they may be notified when the mouse is captured.

Finally, it is up to the user code to actually call this `function component` to indicate whether the application wants to capture the mouse or not.