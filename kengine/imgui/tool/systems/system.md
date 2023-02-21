# [system](system.hpp)

System that displays a "Tools" item in ImGui's main menu bar, which lists all entities with an [tool](../data/tool.md).

Clicking an item in the "Tools" dropdown will toggle the `tool`'s `enabled` attribute. Tools may then use this attribute to know if the user wants them to be displayed or not.

## Example

```cpp
const auto e = r.create();
r.emplace<core::name>(e, "My tool");

auto & tool = r.emplace<imgui::tool::tool>();
tool.enabled = true;

r.emplace<main_loop::execute>(e, [&](float delta_time) {
    if (!tool.enabled) // May be set to false by the system
        return;

    if (ImGui::Begin("My window", &tool.enabled)) // This will make the ImGui window have a "close" button
        ImGui::Text("Content");
    ImGui::End();
});
```