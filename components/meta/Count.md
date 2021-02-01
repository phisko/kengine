# [AttachTo](AttachTo.hpp)

`Meta Component` that returns the number of `Entities` which have the parent `Component`.

## Prototype

```cpp
size_t ();
```

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerCount](../../helpers/meta/registerCount.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Count` `meta Component` for them.