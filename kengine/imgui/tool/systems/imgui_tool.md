# [imgui_tool](imgui_tool.hpp)

System that displays a "Tools" item in ImGui's main menu bar, which lists all entities with an [imgui_tool](../../data/imgui_tool.md).

Clicking an item in the "Tools" dropdown will toggle the `imgui_tool`'s `enabled` attribute. Tools may then use this attribute to know if the user wants them to be displayed or not.

## Example

```cpp
// ... add graphics system handling ImGui
systems::add_imgui_tool(r);

const auto e = r.create();
r.emplace<data::name>(e, "My tool");

auto & tool = r.emplace<data::imgui_tool>();
tool.enabled = true;

r.emplace<functions::execute>(e, [&](float delta_time) {
    if (!tool.enabled) // May be set to false by the ImGuiToolSystem
        return;

    if (ImGui::Begin("My window", &tool.enabled)) // This will make the ImGui window have a "close" button
        ImGui::Text("Content");
    ImGui::End();
});
```
