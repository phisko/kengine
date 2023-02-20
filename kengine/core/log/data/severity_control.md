# [severity_control](severity_control.hpp)

Component that controls which severities the entity's [on_log](../functions/on_log.md) function will be called for.

The [parse_command_line_severity](../helpers/parse_command_line_severity.md) can be used to get the user-specified controls from the command-line.

## Members

### global_severity

```cpp
severity global_severity = severity::log;
```

The maximum verbosity that will be logged. Any message more verbose than this will be ignored, unless a specific severity was specified for its category in `category_severities`.

### category_severities

```cpp
std::unordered_map<std::string, severity> category_severities;
```

Category-specific severities, overriding `global_severity`.

### passes

```cpp
bool passes(const event & event) const noexcept;
```

Returns whether `event` should be logged or not. Used internally by the [log](../helpers/log.md) helper.