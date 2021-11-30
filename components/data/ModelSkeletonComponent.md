# [ModelSkeletonComponent](ModelSkeletonComponent.hpp)

`Component` that lists the bones contained in a given model. Attached to a ["model" Entity](ModelComponent.md).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Initialized by model-loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and then used by other systems to query the names of bones

## Members

### meshes

```cpp
struct Mesh {
    std::vector<std::string> boneNames;
};

std::vector<Mesh> meshes;
```

List of all meshes contained in the model.