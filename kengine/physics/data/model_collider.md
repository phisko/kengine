# [model_collider](model_collider.hpp)

Component that describes the colliders for a [model entity](../../model/).

## Members

### collider type

```cpp
struct collider {
    enum shape {
        box,
        capsule,
        cone,
        cylinder,
        sphere
    };

    shape shape;
    putils::string<KENGINE_BONE_NAME_MAX_LENGTH> bone_name;
    core::transform transform;
};
```

The maximum length of a bone name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_BONE_NAME_MAX_LENGTH` macro.

### colliders

```cpp
std::vector<collider> colliders;
```