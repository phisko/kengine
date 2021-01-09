# [InputBufferComponent](InputBufferComponent.hpp)

`Component` that lets entities receive input events.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by the [InputSystem](../../systems/input/InputSystem.md), filled in by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### keys

```cpp
struct KeyEvent {
    Entity::ID window;
    int key;
    bool pressed;
};
std::vector<KeyEvent> keys;
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
std::vector<ClickEvent> clicks;
```

Click events for the current frame.

### moves

```cpp
struct MouseMoveEvent {
    Entity::ID window;
    putils::Point2f pos;
    putils::Point2f rel;
};
std::vector<MouseMoveEvent> moves;
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
std::vector<MouseScrollEvent> scrolls;
```

Mouse scroll events for the current frame.