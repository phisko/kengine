# [imgui_entity_selector](imgui_entity_selector.hpp)

System that renders an ImGui window that lets users search for and [select](../../data/selected.md) entities.

## Usage

To make a component be taken into account in the search, its [display_imgui](../../meta/display_imgui.md) and [match_string](../../meta/match_string.md) `meta components` must have been registered. Its [type entity](../../helpers/type_helper.md) must also have a [name](../../data/name.md).