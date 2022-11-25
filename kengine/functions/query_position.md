# [query_position](query_position.hpp)

`Function component` that iterates over all entities within a certain radius of a given position.

## Prototype

```cpp
void (const putils::point3f & pos, float radius, const entity_iterator_func & func);
```

### Parameters

* `pos`: the center of the area to be inspected
* `radius`: the maximum distance an entity should be from `pos` to be considered in the area
* `func`: function that will be called for each entity. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_ITERATOR_FUNC_SIZE` macro

## Usage

This `function component` is typically implemented in physics systems (such as the [bullet system](../systems/bullet/bullet.md)), and can be called by user-defined `systems`.