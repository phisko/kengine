# [attach_to](attach_to.hpp)

`Meta component` that attaches the parent component to an entity.

## Prototype

```cpp
void (entt::handle e);
```

### Parameters

* `e`: entity to which the component should be attached

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A helper [register_attach_to](../helpers/meta/impl/register_attach_to.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.