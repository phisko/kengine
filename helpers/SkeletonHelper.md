# [SkeletonHelper](SkeletonHelper.hpp)

Helper functions to manipulate [SkeletonComponents](../components/data/SkeletonComponent.md) and [ModelSkeletonComponents](../components/data/ModelSkeletonComponent.md).

## Members

### getBoneIndex

```cpp
struct BoneIndexes {
    unsigned int meshIndex = 0;
    unsigned int boneIndex = 0;
};
BoneIndexes getBoneIndex(const char * bone, const ModelSkeletonComponent & model);
```

Returns the index of the first bone with the given name, along with the index of the mesh it was found in.

### getBoneMatrix

```cpp
glm::mat4 getBoneMatrix(const char * bone, const SkeletonComponent & skeleton, const ModelSkeletonComponent & model);
```

Returns the mesh-space matrix for a given bone.

### setBoneMatrix

```cpp
void setBoneMatrix(const char * bone, const glm::mat4 & m, SkeletonComponent & skeleton, const ModelSkeletonComponent & model);
```

Sets the mesh-space matrix for a given bone.