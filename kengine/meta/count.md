# [count](count.hpp)

`Meta component` that returns the number of entities which have the parent component.

## Prototype

```cpp
size_t (entt::registry & r);
```

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A helper [register_count](../helpers/meta/impl/register_count.md) function is provided which takes as a template parameter a set of component types and implements this `meta component` for them.