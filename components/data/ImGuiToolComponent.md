# [ImGuiToolComponent](ImGuiToolComponent.hpp)

`Component` that marks an `Entity` as a "tool", i.e. a window that the user may want to enable and disable. These tools can then be controlled through ImGui's "MainMenuBar" thanks to the [ImGuiToolSystem](../../systems/imgui_tool/ImGuiToolSystem.md).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by the [ImGuiToolSystem](../../systems/imgui_tool/ImGuiToolSystem.md).


## Members

### enabled

```cpp
bool enabled = false;
```
