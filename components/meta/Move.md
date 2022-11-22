# [Move](Move.hpp)

`Meta Component` that moves the parent `Component` from one `Entity` to another.

## Prototype

```cpp
void (const Entity & src, const Entity & dst);
```

### Parameters

* `src`: `Entity` to move from
* `dst`: `Entity` to move to

## Usage

It is up to the user to implement this `meta Component` for the `Component` types they wish to be able to use it with.

A helper [registerMove](../../helpers/meta/impl/registerMove.md) function is provided which takes as a template parameter a set of `Component` types and implements the `Move` `meta Component` for them.
