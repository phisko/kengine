# [count](count.hpp)

`Meta component` that returns the number of entities which have the parent component.

## Prototype

```cpp
size_t (entt::registry & r);
```

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A [standard implementation](../helpers/impl/count.md) is provided.