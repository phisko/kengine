# [ImGuiEntitySelectorSystem](ImGuiEntitySelectorSystem.hpp)

`System` that renders an ImGui window that lets users search for and select `Entities` (which will attach a [SelectedComponent](../components/SelectedComponent.hpp) to them).

## Usage

To make a `Component` be taken into account in the search, its [DisplayImGui](../components/meta/DisplayImGui.md) and [MatchString]("../components/meta/MatchString.md) `meta Components`. Its [type Entity](../helpers/TypeHelper.md) must also have a [NameComponent](../components/data/NameComponent.md).

Sample implementations for these can be registered by calling [registerComponentEditor](../helpers/RegisterComponentEditor.md) and [registerComponentMatcher](../helpers/RegisterComponentMatcher.md).