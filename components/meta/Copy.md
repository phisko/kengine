# [Has](Has.hpp)

`Meta Component` that returns whether an `Entity` has the parent `Component`.

## Prototype

```cpp
bool (const Entity & e);
```

### Return value

Whether `e` has the parent `Component`.

### Parameters

* `e`: `Entity` to inspect

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerHas](../../helpers/meta/impl/registerHas.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Has` `meta Component` for them.
