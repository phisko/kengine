# [DetachFrom](DetachFrom.hpp)

`Meta Component` that detaches the parent `Component` from an `Entity`.

## Prototype

```cpp
void (Entity & e);
```

### Parameters

* `e`: `Entity` from which the `Component` should be detached

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerComponentFunctions](../../helpers/RegisterComponentFunctions.md) function is provided which takes as a template parameter a `Component` type and implements the [AttachTo](AttachTo.md), `DetachFrom` and [Has](Has.md) `meta Components` for it.