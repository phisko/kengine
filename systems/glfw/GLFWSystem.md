# [GLFWSystem](GLFWSystem.hpp)

System that creates GLFW windows into [GLFWWindowComponents](../../components/data/GLFWWindowComponent.md) for `Entities` that have a [WindowComponent](../../components/data/WindowComponent.md) and a `GLFWWindowInitComponent`.

The system also takes care of handling window events such as input and close, and captures the mouse when requested through [OnMouseCaptured](../../components/functions/OnMouseCaptured.md).