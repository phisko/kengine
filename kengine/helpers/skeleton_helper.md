# [skeleton_helper](skeleton_helper.hpp)

Helper functions to manipulate [skeleton](../data/skeleton.md) and [model_skeleton](../data/model_skeleton.md) components.

## Members

### get_bone_index

```cpp
struct bone_indices {
    unsigned int mesh_index = 0;
    unsigned int bone_index = 0;
};
bone_indices get_bone_index(const entt::registry & r, const char * bone, const data::model_skeleton & model);
```

Returns the index of the first bone with the given name, along with the index of the mesh it was found in.

### get_bone_matrix

```cpp
glm::mat4 get_bone_matrix(const entt::registry & r, const char * bone, const data::skeleton & skeleton, const data::model_skeleton & model);
```

Returns the mesh-space matrix for a given bone.

### set_bone_matrix

```cpp
void set_bone_matrix(const entt::registry & r, const char * bone, const glm::mat4 & m, data::skeleton & skeleton, const data::model_skeleton & model);
```

Sets the mesh-space matrix for a given bone.
