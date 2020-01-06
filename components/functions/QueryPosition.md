# [QueryPosition](QueryPosition.hpp)

`Function Component` used to get a list of `Entities` within a certain radius of a given position.

## Prototype

```cpp
putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS> (const putils::Point3f & pos, float radius);
```

### Return value

A vector of `Entity` IDs found within the specified area. The maximum number of `Entities` returned defaults to 64 and can be adjusted by defining the `KENGINE_QUERY_POSITION_MAX_RESULTS` macro.

### Parameters

* `pos`: the center of the area to be inspected
* `radius`: the maximum distance an `Entity` should be from `pos` to be considered in the area

## Usage

This `function Component` is typically implemented in physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md)), and can be called by user-defined `systems`.