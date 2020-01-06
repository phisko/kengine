# [ImGuiToolSystem](ImGuiToolSystem.hpp)

`System` that displays a "Tools" item in ImGui's main menu bar, which lists all `Entities` with an [ImGuiToolComponent](../components/data/ImGuiToolComponent.md).

Clicking an item in "Tools" dropdown will toggle the `ImGuiToolComponent`'s `enabled` attribute, letting tools use this attribute to know if the user wants the tool to be displayed or not.

## Example

```cpp
EntityManager em;

// ... add graphics system handling ImGui
em += ImGuiToolSystem(em); // Add the tool system

em += [](Entity & e) {
    auto & tool = e.attach<ImGuiToolComponent>();
    tool.name = "My tool";
    tool.enabled = true;

    e += ImGuiComponent([] {
        if (!tool.enabled) // May be set to false by the ImGuiToolSystem
            return;

        if (ImGui::Begin("My window", &tool.enabled)) // This will make the ImGui window have a "close" button
            ImGui::Text("Content");
        ImGui::End();
    });
};
```