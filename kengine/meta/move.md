# [move](move.hpp)

`Meta component` that moves the parent component from one entity to another.

## Prototype

```cpp
void (entt::handle src, entt::handle dst);
```

### Parameters

* `src`: entity to move from
* `dst`: entity to move to

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A helper [register_move](../helpers/meta/impl/register_move.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.
