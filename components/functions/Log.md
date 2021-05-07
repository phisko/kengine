# [Log](Log.hpp)

`Function Component` that logs a message, with a certain severity and category.

## Protoype

```cpp
void (LogSeverity severity, const char * category, const char * message);
```

## Severity

```cpp
enum class LogSeverity {
    Verbose,
    Log,
    Warning,
    Error
};
```

## Usage

Users wanting to log messages should call [kengine_log](../../helpers/logHelper.md), which will call this `function Component` on all `Entities` which have it.