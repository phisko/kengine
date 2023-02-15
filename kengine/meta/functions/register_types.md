# [register_types](register_types.hpp)

`Function component` that registers some types with a `registry`. The function will typically call some template instantiations of [register_everything](../helpers/register_everything.md). `register_everything` might call these functions in parallel. Any required shared state should be created beforehand using `pre_register_types` (see below).

## Protoype

```cpp
void (entt::registry & r);
```

### Parameters

* `r`: `registry` with which the types should be registered

## Usage

This function is called by [register_all_types](../helpers/register_all_types.md). This can be used when creating a new (or temporary) `registry` which needs to be aware of all reflectible types.

# pre_register_types

`Function` component that "pre-registers" some types with a `registry`. Pre-registration should do all the required operations to allow registration (with `register_types`) to work properly in a parallel context.