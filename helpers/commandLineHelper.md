# [commandLineHelper](commandLineHelper.hpp)

Helper function to parse command-line arguments from [CommandLineComponents](../components/data/CommandLineComponent.md).

## parseCommandLine

```cpp
template<typename T>
T parseCommandLine() noexcept;
```

Finds the first entity with a `CommandLineComponent` and parses its `arguments` vector into a `T`. For more details on the API, see [putils' command_line_arguments helper](https://github.com/phisko/putils/blob/master/command_line_arguments.md).