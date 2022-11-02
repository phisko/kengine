# [commandLineHelper](commandLineHelper.hpp)

Helper functions to manipulate [CommandLineComponents](../components/data/CommandLineComponent.md).

## createCommandLineEntity

```cpp
void createCommandLineEntity(int argc, const char ** argv) noexcept;
```

Creates an `Entity` and attaches a `CommandLineComponent` to it, initialized with the provided arguments.

## parseCommandLine

```cpp
template<typename T>
T parseCommandLine() noexcept;
```

Parses command-line arguments from [CommandLineComponents](../components/data/CommandLineComponent.md).

Finds the first entity with a `CommandLineComponent` and parses its `arguments` vector into a `T`. For more details on the API, see [putils' command_line_arguments helper](https://github.com/phisko/putils/blob/master/command_line_arguments.md).