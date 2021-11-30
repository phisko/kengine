# [InputComponent](InputComponent.hpp)

`Component` that lets entities receive input events.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable as it contains functors
* Processed by the [InputSystem](../../systems/input/InputSystem.md)

## Members

### onKey

```cpp
putils::function<void(EntityID window, int keycode, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onKey;
```

### onMouseMove

```cpp
putils::function<void(EntityID window, const putils::Point2f & screenCoordinates, const putils::Point2f & relativeMovement), KENGINE_INPUT_FUNCTION_SIZE> onMouseMove;
```

### onMouseButton

```cpp
putils::function<void(EntityID window, int button, const putils::Point2f & screenCoordinates, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onMouseButton;
```

### onScroll

```cpp
putils::function<void(EntityID window, float xoffset, float yoffset, const putils::Point2f & screenCoordinates), KENGINE_INPUT_FUNCTION_SIZE)> onScroll;
```

All these callbacks take as a first parameter the `id` of the [window entity](WindowComponent.md) in which the event happened.

The maximum size for these functors defaults to 64 and can be adjusted by defining the `KENGINE_INPUT_FUNCTION_SIZE` macro.