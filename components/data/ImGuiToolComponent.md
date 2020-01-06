# [ImGuiToolComponent](ImGuiToolComponent.hpp)

`Component` that marks an `Entity`'s [ImGuiComponent](ImGuiComponent.md) as a "tool", i.e. a window that the user may want to enable and disable. These tools can then be controlled through ImGui's "MainMenuBar" thanks to the [ImGuiToolSystem](../../systems/ImGuiToolSystem.md).

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### enabled

```cpp
bool enabled = true;
```