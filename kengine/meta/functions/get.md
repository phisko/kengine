# [get](get.hpp)

`Meta component` that returns a pointer to a given entity's parent component.

## Prototype

```cpp
void * (entt::handle e);
```

### Return value

A pointer to the parent component attached to `e`.

### Parameters

* `e`: entity to inspect

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A helper [register_get](../helpers/meta/impl/register_get.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.
