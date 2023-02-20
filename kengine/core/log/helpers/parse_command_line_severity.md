# [parse_command_line_severity](parse_command_line_severity.hpp)

```cpp
log_severity_control parse_command_line_severity() noexcept;
```

Parses the command-line for:
* a `--log_level` parameter indicating the maximum log verbosity desired by the user
* a `--log_category_levels` parameter specifying category-specific verbosities (under the format `--log_category_levels=first_category:verbose,second_category:very_verbose,third_category:warning`)
