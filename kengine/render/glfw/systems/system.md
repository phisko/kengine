# [system](system.hpp)

System that creates GLFW windows into [glfw window](../data/window.md) components for entities that have [window](../../data/window.md) and `glfw window_init` components.

The system also takes care of handling window events such as input and close, and captures the mouse when requested through [on_mouse_captured](../../functions/on_mouse_captured.md).