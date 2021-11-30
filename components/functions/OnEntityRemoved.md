# [OnEntityRemoved](OnEntityRemoved.hpp)

`Function Component` used as a callback when an `Entity` is removed.

## Prototype

```cpp
void (Entity & e);
```

### Parameters

* `e`: `Entity` that is about to be removed

## Usage

The engine automatically calls this `function Component` whenever an `Entity` is removed.