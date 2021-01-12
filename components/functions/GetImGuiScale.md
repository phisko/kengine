# [GetImGuiScale](GetImGuiScale.hpp)

`Function Component` that returns the user-defined scale for ImGui elements.

## Prototype

```cpp
float ();
```

### Return value

Scale for ImGui elements, which tool developers may want to use to properly scale their ImGui windows.

## Usage

Graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)) sometimes offer an [adjustable](AdjustableComponent.md) that lets users scale ImGui elements to their liking. They can then implement this `function Component` so that other `systems` or tools may query the desired scale.