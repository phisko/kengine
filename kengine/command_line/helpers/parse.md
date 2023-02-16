# [parse](parse.hpp)

```cpp
template<typename T>
T parse(const entt::registry & r) noexcept;
```

Parses [command-line arguments](../data/arguments.md).

Finds the first "command-line entity" and parses its `args` vector into a `T`. For more details on the API, see [putils' command_line_arguments helper](https://github.com/phisko/putils/blob/master/putils/command_line_arguments.md).
