# [adjustable](adjustable.hpp)

Utility component exposing values that may be adjusted at runtime, e.g. by the [imgui_adjustable](../imgui_adjustable/systems/imgui_adjustable.md) system.

Currently supported types are `bool`, `int`, `float`, [putils::normalized_color](https://github.com/phisko/putils/blob/master/putils/color.md) and `enum` types.

The maximum length of the adjustable's name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_ADJUSTABLE_NAME_MAX_LENGTH` macro.

## Usage

```c++
static bool global_bool = false;
static int global_int = 42;

const auto e = registry.create();
registry.emplace<kengine::data::adjustable>(e) = {
        "Globals", // Category for these adjustables
        { // vector of adjustables
            { "Bool name", &global_bool },
            { "Int name", &global_int }
        }
};

// ... Later on, once adjustable system has been processed

std::cout << global_bool << std::endl; // May be true, if user modified it
```