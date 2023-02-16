# [command_line](command_line.hpp)

Component holding the command-line arguments received by the program. Can be created and queried using the [command_line_helper](../helpers/command_line_helper.md).

## Members

### arguments

```cpp
std::vector<std::string_view> arguments;
```

The command-line arguments, as received by the program's `main` function.