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

A helper [registerDetachFrom](../../helpers/meta/impl/registerDetachFrom.md) function is provided which takes as a template parameter a set of `Component` types and implements the `DetachFrom` `meta Component` for them.