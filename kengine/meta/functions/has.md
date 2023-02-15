# [has](has.hpp)

`Meta component` that returns whether an entity has the parent component.

## Prototype

```cpp
bool (entt::const_handle e);
```

### Return value

Whether `e` has the parent component.

### Parameters

* `e`: entity to inspect

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A [standard implementation](../helpers/impl/has.md) is provided.
