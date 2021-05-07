# [logHelper](logHelper.hpp)

Helper functions for message logging.

## log

```cpp
void log(LogSeverity severity, const char * category, const char * message) noexcept;
```

Calls the [Log](../components/functions/Log.md) `function Component` on all `Entities` which have it.

## kengine_log

```cpp
#define kengine_log(severity, category, message)
```

Helper macro for logging, simply calls `log`.

## kengine_logf

```cpp
#define kengine_logf(severity, category, format, ...)
```

Helper macro for logging. Constructs a [putils::string<1024>](../putils/string.md) and fills it with printf-style formatting, then calls `log`.