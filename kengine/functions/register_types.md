# [register_types](register_types.hpp)

`Function component` that registers some types with a `registry`. The function will typically call some template instantiations of [register_everything](../helpers/meta/register_everything.md).

## Protoype

```cpp
void (entt::registry & r);
```

### Parameters

* `r`: `registry` with which the types should be registered

## Usage

This function is called by [register_all_types](../helpers/register_all_types.md). This can be used when creating a new (or temporary) `registry` which needs to be aware of all reflectible types.