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

A helper [register_remove](../helpers/meta/impl/register_remove.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.