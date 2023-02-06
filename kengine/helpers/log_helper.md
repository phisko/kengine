# [log_helper](log_helper.hpp)

Helper functions for message logging.

## Functions

### log

```cpp
void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept;
```

Calls the [log](../functions/log.md) `function component` on all entities which have it.

If `severity` is lower or equal to the maximum log severity that has been requested, nothing will be done.

The requested log severity is determined by what `parse_command_line_severity` returns, or by calls to `set_minimum_log_severity`.

### parse_command_line_severity

```cpp
log_severity parse_command_line_severity() noexcept;
```

Parses the command-line for a `--log_level` parameter indicating the maximum log verbosity desired by the user.

### set_minimum_log_severity

```cpp
void set_minimum_log_severity(log_severity severity) noexcept;
```

Ensures that messages with the requested severity will be logged.

## Macros

### kengine_log

```cpp
#define kengine_log(r, severity, category, message)
```

Helper macro for logging, calls `log` if `severity` is lower or equal to `KENGINE_LOG_MAX_SEVERITY`. `KENGINE_LOG_MAX_SEVERITY` defaults to `all`, and can be specified as a CMake option or defined at compile-time.

### kengine_logf

```cpp
#define kengine_logf(r, severity, category, format, ...)
```

Helper macro for logging. Constructs a [putils::string<1024>](https://github.com/phisko/putils/blob/master/putils/string.md) and fills it with printf-style formatting, then calls `log`.