# [command_line_helper](command_line_helper.hpp)

Helper functions to manipulate the [command_line](../data/command_line.md).

## create_command_line_entity

```cpp
void create_command_line_entity(entt::registry & r, int argc, const char ** argv) noexcept;
```

Creates an entity and attaches a `command_line` to it, initialized with the provided arguments.

## parse_command_line

```cpp
template<typename T>
T parse_command_line(const entt::registry & r) noexcept;
```

Parses command-line arguments from the [command_line](../data/command_line.md).

Finds the first entity with a `command_line` and parses its `arguments` vector into a `T`. For more details on the API, see [putils' command_line_arguments helper](https://github.com/phisko/putils/blob/master/putils/command_line_arguments.md).