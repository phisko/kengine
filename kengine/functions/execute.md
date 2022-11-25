# [execute](execute.hpp)

`Function component` that gets called each frame.

## Protoype

```cpp
void (float delta_time);
```

### Parameters

* `delta_time`: time since last frame, in seconds

## Usage

Most systems have this `function component` and use it to perform operations on the entities they are interested in.

This `function component` may also be used to give a single entity some specific behavior.