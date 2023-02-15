# [emplace_or_replace](emplace_or_replace.hpp)

`Meta component` that attaches the parent component to an entity.

## Prototype

```cpp
void (entt::handle e, const void * comp);
```

### Parameters

* `e`: entity to which the component should be attached
* `comp`: pointer to an instance of the parent component to copy from. Can be `nullptr`

## Usage

It is up to the user to implement this `meta component` for the component types they wish to be able to use it with.

A [standard implementation](../helpers/impl/emplace_or_replace.md) is provided.

## Move alternative

An `emplace_or_replace_move` meta component is also provided, which takes a non-`const` pointer as parameter and will move from it.