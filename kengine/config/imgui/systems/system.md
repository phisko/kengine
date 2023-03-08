# [system](system.hpp)

System that renders an ImGui window with editing tools for [config](../../data/configurable.md) components.

An config value can specify a category by formatting its `name` like so:
```
[Category/Subcategory/...] Name
```
Categories are then displayed together in a tree format.