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

## Severity control

```cpp
struct log_severity_control {
    log_severity global_severity = log_severity::log;
    std::unordered_map<std::string, log_severity> category_severities;

    bool passes(const log_event & event) noexcept;
};
```

Can be used by logging systems to filter out log events based on their verbosities. If a verbosity is specified for a given category, it overrides the global severity.

`passes` returns true if the event should be logged.

[log_helper::parse_command_line_severity](../helpers/log_helper.md) can be used to parse this structure from the command-line.

## Usage

Users wanting to log messages should call [kengine_log](../helpers/log_helper.md), which will call this `function component` on all entities which have it.