# [SkeletonComponent](SkeletonComponent.hpp)

`Component` that holds transformation matrices for an `Entity`'s bones.

## Specs

* Not [reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Initialized by model loading systems and used by physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md)) (among others)

## Members

### Mesh type

```cpp
struct Mesh {
    glm::mat4 boneMatsBoneSpace[KENGINE_SKELETON_MAX_BONES]; // Used by shader
    glm::mat4 boneMatsMeshSpace[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space
};
```

Represents a single mesh in the model.

`boneMatsBoneSpace` holds the matrices for each bone in "bone space", i.e. relative to the bone's identity matrix.

`boneMatsMeshSpace` holds the matrices for each bone in "mesh space", i.e. relative to the `Entity`'s position.

### meshes

```cpp
std::vector<Mesh> meshes;
```

List of all the meshes comprising the model.