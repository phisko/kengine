# [input](input.hpp)

Component that receives input events.

## Members

### on_key

```cpp
putils::function<void(entt::entity window, int keycode, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> on_key;
```

### on_mouse_move

```cpp
putils::function<void(entt::entity window, const putils::point2f & screen_coordinates, const putils::point2f & relative_movement), KENGINE_INPUT_FUNCTION_SIZE> on_mouse_move;
```

### on_mouse_button

```cpp
putils::function<void(entt::entity window, int button, const putils::point2f & screen_coordinates, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> on_mouse_button;
```

### on_scroll

```cpp
putils::function<void(entt::entity window, float xoffset, float yoffset, const putils::point2f & screen_coordinates), KENGINE_INPUT_FUNCTION_SIZE)> on_scroll;
```

All these callbacks take as a first parameter the [window entity](../../render/data/window.md) in which the event happened.

The maximum size for these functors defaults to 64 and can be adjusted by defining the `KENGINE_INPUT_FUNCTION_SIZE` macro.