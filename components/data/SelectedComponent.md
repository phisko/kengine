# [SelectedComponent](SelectedComponent.hpp)

"Tag" `Component` that marks an `Entity` as "selected" (i.e. currently under inspection by a user).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (empty)
* Used by the [ImGuiEntityEditorSystem](../../systems/imgui_entity_editor/ImGuiEntityEditorSystem.md) and the [ImGuiEntitySelectorSystem](../../systems/imgui_entity_selector/ImGuiEntitySelectorSystem.md)