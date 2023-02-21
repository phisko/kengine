# [buffer](buffer.hpp)

Component that stores input events. These are sent by graphics systems (or whatever else is used to capture input). The [input system](../systems/system.md) then processes the `buffer` to send the buffered events to [input handlers](handler.md).

## Members

### keys

```cpp
struct key_event {
    entt::entity window;
    int key;
    bool pressed;
};
std::vector<key_event> keys;
```

Buffered key events.

### clicks

```cpp
struct click_event {
    entt::entity window;
    putils::point2f pos;
    int button;
    bool pressed;
};
std::vector<click_event> clicks;
```

Buffered mouse click events.

### moves

```cpp
struct mouse_move_event {
    entt::entity window;
    putils::point2f pos;
    putils::point2f rel;
};
std::vector<mouse_move_event> moves;
```

Buffered mouse move events.

### scrolls

```cpp
struct mouse_scroll_event {
    entt::entity window;
    float xoffset;
    float yoffset;
    putils::point2f pos;
};
std::vector<mouse_scroll_event> scrolls;
```

Buffered mouse scroll events.