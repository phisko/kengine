# [Get](Get.hpp)

`Meta Component` that returns a pointer to a given `Entity`'s parent `Component`.

## Prototype

```cpp
void * (const Entity & e);
```

### Return value

A pointer to the parent `Component` attached to `e`.

### Parameters

* `e`: `Entity` to inspect

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerGet](../../helpers/meta/impl/registerGet.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Get` `meta Component` for them.
