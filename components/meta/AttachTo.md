# [AttachTo](AttachTo.hpp)

`Meta Component` that attaches the parent `Component` to an `Entity`.

## Prototype

```cpp
void (Entity & e);
```

### Parameters

* `e`: `Entity` to which the `Component` should be attached

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerComponentFunctions](../../helpers/RegisterComponentFunctions.md) function is provided which takes as a template parameter a `Component` type and implements the `AttachTo`, [DetachFrom](DetachFrom.md) and [Has](Has.md) `meta Components` for it.