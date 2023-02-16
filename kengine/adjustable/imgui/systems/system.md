# [imgui_adjustable](imgui_adjustable.hpp)

System that renders an ImGui window with editing tools for [adjustable](../../data/adjustable.md) components.

An adjustable value can specify a category by formatting its `name` like so:
```
[Category/Subcategory/...] Name
```
Categories are then displayed together in a tree format.

## Serialization

Saving and loading to CSV are also implemented.

The directory in which the CSV file is saved defaults to "." and can be adjusted by defining the `KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH` macro.

The name of the CSV file defaults to "adjust.cnf" and can be adjusted by defining the `KENGINE_ADJUSTABLE_SAVE_FILE` macro.

The name of the CSV file defaults to "adjust.cnf" and can be adjusted by defining the `KENGINE_ADJUSTABLE_SEPARATOR` macro.

The maximum number of sections in an adjustable's name defaults to 8 and can be adjusted by defining the `KENGINE_MAX_ADJUSTABLE_SECTIONS` macro.

The maximum number of adjustables defaults to 256 and can be adjusted by defining the `KENGINE_MAX_ADJUSTABLES` macro.

The precision of floating point adjustables defaults to 5 and can be adjusted by defining the `KENGINE_ADJUSTABLE_FLOAT_PRECISION` macro.