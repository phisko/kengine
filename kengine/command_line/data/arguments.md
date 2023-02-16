# [arguments](arguments.hpp)

Component holding the command-line arguments received by the program. Can be created and queried using the [helpers](../helpers/).

## Members

### arguments

```cpp
std::vector<std::string_view> arguments;
```

The command-line arguments, as received by the program's `main` function.