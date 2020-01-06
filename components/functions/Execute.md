# [Execute](Execute.hpp)

`Function Component` that gets called each frame.

## Protoype

```cpp
void (float deltaTime);
```

### Parameters

* `deltaTime`: time since last frame, in seconds

## Usage

Most `systems` have this `function Component` and use it to perform operations on the `Entities` they are interested in.

This `function Component` may also be used to give a single `Entity` some specific behavior.