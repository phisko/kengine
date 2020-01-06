# [ImGuiEntityEditorSystem](ImGuiEntityEditorSystem.hpp)

`System` that renders ImGui windows with editing tools for entities with a [SelectedComponent](../components/data/SelectedComponent.md).

## Usage

To make a `Component` appear in the editor, its [EditImGui](../components/meta/EditImGui.md) `meta Component` must first be implemented. Its [type Entity](../helpers/TypeHelper.md) must also have a [NameComponent](../components/data/NameComponent.md).

A sample implementation can be registered by calling [registerComponentEditor](../helpers/RegisterComponentEditor.md).