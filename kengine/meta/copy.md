# [copy](copy.hpp)

`Meta component` that copies the parent component from one entity to another.

## Prototype

```cpp
void (entt::const_handle src, entt::handle dst);
```

### Parameters

* `src`: entity to copy from
* `dst`: entity to copy to

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A helper [register_copy](../helpers/meta/impl/register_copy.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.
