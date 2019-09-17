# [ImGuiEntityEditorSystem](ImGuiEntityEditorSystem.hpp)

`System` that renders ImGui windows with editing tools for entities with a [SelectedComponent](../components/SelectedComponent.hpp).

To make a `Component` appear in the editor, it must first be registered by calling [kengine::registerComponentEditor](../helpers/RegisterComponentEditor.hpp).