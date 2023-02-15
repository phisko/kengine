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

A [standard implementation](../helpers/impl/get.md) is provided.
