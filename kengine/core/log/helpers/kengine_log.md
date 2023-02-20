# [kengine_log](kengine_log.hpp)

Helper macros for message logging.

## kengine_log

```cpp
#define kengine_log(r, severity, category, message)
```

Helper macro for logging, calls [log](../helpers/log.md) if `severity` is lower or equal to `KENGINE_LOG_MAX_SEVERITY`. `KENGINE_LOG_MAX_SEVERITY` defaults to `all`, and can be specified as a CMake option or defined at compile-time.

## kengine_logf

```cpp
#define kengine_logf(r, severity, category, format, ...)
```

Helper macro for logging. Constructs a [putils::string<1024>](https://github.com/phisko/putils/blob/master/putils/string.md) and fills it with printf-style formatting, then calls `kengine_log`.