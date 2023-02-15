# [imgui_entity_selector](imgui_entity_selector.hpp)

System that renders an ImGui window that lets users search for and [select](../../../core/data/selected.md) entities.

## Usage

To make a component be taken into account in the search, its [display_imgui](../../../meta/functions//display_imgui.md) and [match_string](../../../meta/functions//match_string.md) `meta components` must have been registered. Its [type entity](../../../meta/helpers/type_helper.md) must also have a [name](../../../core/data/name.md).