# [on_log](on_log.hpp)

`Function component` that logs a message, with a certain severity and category.

## Protoype

```cpp
void (const event & e);
```

Takes an [event](../helpers/event.md) as argument.

## Usage

Users wanting to log messages should call [kengine_log](../helpers/kengine_log.md), which will call this `function component` on all entities which have it.