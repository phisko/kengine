# [Copy](Copy.hpp)

`Meta Component` that copies the parent `Component` from one `Entity` to another.

## Prototype

```cpp
void (const Entity & src, const Entity & dst);
```

### Parameters

* `src`: `Entity` to copy from
* `dst`: `Entity` to copy to

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerCopy](../../helpers/meta/impl/registerCopy.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Copy` `meta Component` for them.
