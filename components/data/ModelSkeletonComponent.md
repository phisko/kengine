# [ModelSkeletonComponent](ModelSkeletonComponent.hpp)

`Component` that lists the bones contained in a given model. Attached to a ["model" Entity](ModelComponent.md).

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Initialized by model-loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and then used by other systems to query the names of bones

## Members

### Mesh type

```cpp
struct Mesh {
    std::vector<std::string> boneNames;

    putils_reflection_class_name(ModelSkeletonComponentMesh);
    putils_reflection_attributes(
        putils_reflection_attribute(&Mesh::boneNames)
    );
};
```

Provides the names for bones in a given mesh.

### meshes

```cpp
std::vector<Mesh> meshes;
```

List of all meshes contained in the model.