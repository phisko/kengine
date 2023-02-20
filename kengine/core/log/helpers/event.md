# [event](event.hpp)

Represents a log event.

## Members

### severity

```cpp
severity severity;
```

The event's [severity](severity.md).

### category

```cpp
const char * category;
```

The event's category, generally the name of the library which emitted it.

### message

```cpp
const char * message;
```

The event's message to be logged.