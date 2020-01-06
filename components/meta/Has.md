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

A helper [registerComponentFunctions](../../helpers/RegisterComponentFunctions.md) function is provided which takes as a template parameter a `Component` type and implements the [AttachTo](AttachTo.md), [DetachFrom](DetachFrom.md) and `Has` `meta Components` for it.
