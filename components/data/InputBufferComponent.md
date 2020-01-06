# [InputBufferComponent](InputBufferComponent.hpp)

`Component` that lets entities receive input events.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by the [InputSystem](../../systems/InputSystem.md), filled in by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### EventVector type

```cpp
template<typename T>
using EventVector = putils::vector<T, KENGINE_INPUT_MAX_BUFFERED_EVENTS>;
```

This template container is used for all event buffers. The maximum number of events of each type defaults to 128 and can be modified by defining the `KENGINE_INPUT_MAX_BUFFERED_EVENTS` macro.

### keys

```cpp
struct KeyEvent {
    Entity::ID window;
    int key;
    bool pressed;
};
EventVector<KeyEvent> keys;
```

Key events for the current frame.

### clicks

```cpp
struct ClickEvent {
    Entity::ID window;
    putils::Point2f pos;
    int button;
    bool pressed;
};
EventVector<ClickEvent> clicks;
```

Click events for the current frame.

### moves

```cpp
struct MouseMoveEvent {
    Entity::ID window;
    putils::Point2f pos;
    putils::Point2f rel;
};
EventVector<MouseMoveEvent> moves;
```

Mouse move events for the current frame.

### scrolls

```cpp
struct MouseScrollEvent {
    Entity::ID window;
    float xoffset;
    float yoffset;
    putils::Point2f pos;
};
EventVector<MouseScrollEvent> scrolls;
```

Mouse scroll events for the current frame.