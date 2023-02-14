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

A helper [register_has](../helpers/meta/impl/register_has.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.
