# [log](log.hpp)

`Function component` that logs a message, with a certain severity and category.

## Protoype

```cpp
struct log_event {
    log_severity severity;
    const char * category;
    const char * message;
};
void (const log_event & event);
```

## Severity

```cpp
enum class log_severity {
    all,
    verbose,
    log,
    warning,
    error,
    none
};
```

## Usage

Users wanting to log messages should call [kengine_log](../helpers/log_helper.md), which will call this `function component` on all entities which have it.