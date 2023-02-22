# [imgui_entity_selector](imgui_entity_selector.hpp)

System that renders an ImGui window that lets users search for and [select](../../../core/data/selected.md) entities.

## Usage

To make a component be taken into account in the search, its [display_imgui](../../functions/display_imgui.md) and [match_string](../../../functions/match_string.md) `meta components` must have been registered. Its [type entity](../../../helpers/get_type_entity.md) must also have a [name](../../../../core/data/name.md).