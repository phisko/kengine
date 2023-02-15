# [remove](remove.hpp)

`Meta component` that detaches the parent component from an entity.

## Prototype

```cpp
void (entt::handle e);
```

### Parameters

* `e`: entity from which the component should be detached

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A [standard implementation](../helpers/impl/remove.md) is provided.