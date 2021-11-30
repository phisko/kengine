# [ModelColliderComponent](ModelColliderComponent.hpp)

`Component` attached to a ["model" Entity](ModelComponent.md) that describes the colliders associated with the 3D model.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (holds dynamic pointers)
* Processed by physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md))

## Members

### Collider type

```cpp
struct Collider {
    enum Shape {
        Box,
        Capsule,
        Cone,
        Cylinder,
        Sphere
    };

    Shape shape;
    putils::string<KENGINE_BONE_NAME_MAX_LENGTH> boneName;
    TransformComponent transform;
};
```

The maximum length of a bone name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_BONE_NAME_MAX_LENGTH` macro.

### colliders

```cpp
std::vector<Collider> colliders;
```