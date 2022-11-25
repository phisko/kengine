# [log_helper](log_helper.hpp)

Helper functions for message logging.

## log

```cpp
void log(const entt::registry & r, log_severity severity, const char * category, const char * message) noexcept;
```

Calls the [log](../functions/log.md) `function component` on all entities which have it.

## parse_command_line_severity

```cpp
log_severity parse_command_line_severity() noexcept;
```

Parses the command-line for a `--log_level` parameter indicating the maximum log verbosity desired by the user.

## kengine_log

```cpp
#define kengine_log(r, severity, category, message)
```

Helper macro for logging, simply calls `log`.

## kengine_logf

```cpp
#define kengine_logf(r, severity, category, format, ...)
```

Helper macro for logging. Constructs a [putils::string<1024>](https://github.com/phisko/putils/blob/master/putils/string.md) and fills it with printf-style formatting, then calls `log`.