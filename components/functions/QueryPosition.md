# [QueryPosition](QueryPosition.hpp)

`Function Component` used to iterate over all `Entities` within a certain radius of a given position.

## Prototype

```cpp
void (const putils::Point3f & pos, float radius, const EntityIteratorFunc & func);
```

### Parameters

* `pos`: the center of the area to be inspected
* `radius`: the maximum distance an `Entity` should be from `pos` to be considered in the area
* `func`: function that will be called for each `Entity`. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_ITERATOR_FUNC_SIZE` macro

## Usage

This `function Component` is typically implemented in physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md)), and can be called by user-defined `systems`.