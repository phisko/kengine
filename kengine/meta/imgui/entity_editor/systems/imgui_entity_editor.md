# [imgui_entity_editor](imgui_entity_editor.hpp)

System that renders ImGui windows with editing tools for [selected](../../../core/data/selected.md) entities.

## Usage

To make a component appear in the editor, its [edit_imgui](../../../meta/functions/edit_imgui.md) `meta component` must first be implemented. Its [type entity](../../../meta/helpers/type_helper.md) must also have a [name](../../../core/data/name.md).